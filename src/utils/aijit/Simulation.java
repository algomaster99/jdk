/*
 * Workload for the AIJitAgent proof of concept.
 *
 * Each kernel runs in its own thread and its throughput is printed once a
 * second, so a recompilation triggered by the agent shows up as a step in
 * the time series. Every result is compared with the first (interpreted)
 * result: a hint may make code slower, but must never make it wrong.
 *
 *   inline   a hot callee whose cold slow path puts it above FreqInlineSize.
 *            Once inlined, escape analysis removes the Vec allocated on
 *            every iteration.
 *   control  a loop C2 already vectorizes; the agent should leave it alone.
 *
 * Usage: java Simulation [seconds]
 */
import java.util.concurrent.atomic.LongAdder;

public class Simulation {
    record Vec(double x, double y, double z) {}

    static final String[] NAMES = {"inline", "control"};

    static final Vec BASE = new Vec(0.3, -0.2, 0.9);
    static final int[] DATA = new int[4096];

    static {
        int seed = 42;
        for (int i = 0; i < DATA.length; i++) {
            seed = seed * 1103515245 + 12345;
            DATA[i] = seed;
        }
    }

    // The hot path is a few flops, but the rarely taken slow path pushes the
    // bytecode size above FreqInlineSize (325). C2's size check counts cold
    // code too, so it calls step and heap allocates a Vec on every iteration
    // of trajectory.
    static Vec step(Vec p, double t) {
        double x = p.x() + 0.5 * t, y = p.y() - 0.25 * t, z = p.z() * 0.999;
        if (x * x + y * y + z * z < 1e6) {
            return new Vec(x, y, z);
        }
        // Slow path: the point drifted far away; rotate and damp it back.
        // Taylor series for sin/cos so the body stays plain arithmetic.
        double t2 = t * t, t3 = t2 * t, t4 = t3 * t, t5 = t4 * t, t6 = t5 * t, t7 = t6 * t;
        double s = t - t3 / 6.0 + t5 / 120.0 - t7 / 5040.0;
        double c = 1.0 - t2 / 2.0 + t4 / 24.0 - t6 / 720.0;
        // Rotate about z, then about x, then about y.
        double x1 = p.x() * c - p.y() * s;
        double y1 = p.x() * s + p.y() * c;
        double z1 = p.z();
        double y2 = y1 * c - z1 * s;
        double z2 = y1 * s + z1 * c;
        double x3 = x1 * c + z2 * s;
        double z3 = -x1 * s + z2 * c;
        // Damped spring toward the origin plus a small drift.
        double k = 0.001, d = 0.999;
        double x4 = (x3 - k * x3) * d + 0.5 * k * t;
        double y4 = (y2 - k * y2) * d - 0.25 * k * t2;
        double z4 = (z3 - k * z3) * d + 0.125 * k * t3;
        // Shear and a cubic correction.
        double x5 = x4 + 0.01 * y4 - 0.02 * z4 + 1e-3 * x4 * x4 * x4;
        double y5 = y4 - 0.03 * x4 + 0.01 * z4 - 1e-3 * y4 * y4 * y4;
        double z5 = z4 + 0.02 * x4 - 0.01 * y4 + 1e-3 * z4 * z4 * z4;
        // Clamp into a ball of radius 2.
        double r2 = x5 * x5 + y5 * y5 + z5 * z5;
        double scale = r2 > 4.0 ? 2.0 / Math.sqrt(r2) : 1.0;
        return new Vec(x5 * scale, y5 * scale, z5 * scale);
    }

    static double trajectory(Vec base, int n) {
        double acc = 0;
        for (int i = 0; i < n; i++) {
            Vec v = step(base, i * 1e-4);
            acc += v.x() + v.y() * 0.5 + v.z() * 0.25;
        }
        return acc;
    }

    static long sumArray(int[] a) {
        long sum = 0;
        for (int i = 0; i < a.length; i++) {
            sum += a[i];
        }
        return sum;
    }

    // One driver loop per kernel, so each kernel lands in its own compilation
    // root (an OSR nmethod) that the agent can target by name.
    static void runTrajectory() { while (!stop) { record(0, Double.doubleToLongBits(trajectory(BASE, 2_000))); } }
    static void runSumArray()   { while (!stop) { record(1, sumArray(DATA)); } }

    static volatile boolean stop;
    static final long[] EXPECTED = new long[NAMES.length];
    static final LongAdder[] OPS = new LongAdder[NAMES.length];
    static final LongAdder MISMATCHES = new LongAdder();

    static void record(int kernel, long result) {
        if (result != EXPECTED[kernel]) {
            MISMATCHES.increment();
        }
        OPS[kernel].increment();
    }

    public static void main(String[] args) throws Exception {
        int seconds = args.length > 0 ? Integer.parseInt(args[0]) : 20;
        int k = NAMES.length;
        EXPECTED[0] = Double.doubleToLongBits(trajectory(BASE, 2_000));
        EXPECTED[1] = sumArray(DATA);
        Runnable[] loops = {Simulation::runTrajectory, Simulation::runSumArray};
        for (int i = 0; i < k; i++) {
            OPS[i] = new LongAdder();
            Thread t = new Thread(loops[i], NAMES[i]);
            t.setDaemon(true);
            t.start();
        }

        // ops/s per kernel per second; the second half is the steady state.
        double[][] rate = new double[k][seconds];
        System.out.printf("%4s %12s %12s   (ops/s)%n", "t", NAMES[0], NAMES[1]);
        long[] last = new long[k];
        long prev = System.nanoTime();
        for (int s = 0; s < seconds; s++) {
            Thread.sleep(1000);
            long now = System.nanoTime();
            for (int i = 0; i < k; i++) {
                long total = OPS[i].sum();
                rate[i][s] = (total - last[i]) * 1e9 / (now - prev);
                last[i] = total;
            }
            prev = now;
            System.out.printf("%4d %12.0f %12.0f%n", s + 1, rate[0][s], rate[1][s]);
        }
        stop = true;

        System.out.print("steady-state (last half):");
        for (int i = 0; i < k; i++) {
            double sum = 0;
            for (int s = seconds / 2; s < seconds; s++) {
                sum += rate[i][s];
            }
            System.out.printf("  %s=%.0f", NAMES[i], sum / (seconds - seconds / 2));
        }
        System.out.println();
        System.out.println("result mismatches: " + MISMATCHES.sum());
    }
}
