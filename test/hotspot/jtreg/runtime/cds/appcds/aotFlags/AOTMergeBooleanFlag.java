/*
 * @test
 * @summary Basic sanity test that -XX:AOTMerge is accepted as a boolean option
 * @requires vm.cds.supports.aot.class.linking
 * @library /test/lib
 * @run driver AOTMergeBooleanFlag
 */

import jdk.test.lib.cds.CDSTestUtils;
import jdk.test.lib.process.OutputAnalyzer;
import jdk.test.lib.process.ProcessTools;

public class AOTMergeBooleanFlag {

    public static void main(String[] args) throws Exception {
        testFlag("-XX:+AOTMerge");
        testFlag("-XX:-AOTMerge");
    }

    private static void testFlag(String flag) throws Exception {
        ProcessBuilder pb = ProcessTools.createTestJavaProcessBuilder(
            "-Xlog:aot=info",
                "-XX:+UnlockExperimentalVMOptions",
                "-XX:AOTCache=dummy.aot",
                flag,
                "-version");

        OutputAnalyzer out = CDSTestUtils.executeAndLog(pb, "aot-merge-flag");
        out.shouldNotContain("Unrecognized VM option 'AOTMerge'");
        out.shouldHaveExitValue(0);
    }
}
