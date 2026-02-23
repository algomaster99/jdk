/*
 * @test
 * @summary Test that the AOTMerge boolean flag is recognized and that it requires an AOT cache to be specified.
 * @library /test/lib
 * @run driver AOTMergeBooleanFlag
 */

import jdk.test.lib.cds.CDSTestUtils;
import jdk.test.lib.process.OutputAnalyzer;
import jdk.test.lib.process.ProcessTools;

// Execute with
// make run-test TEST=runtime/cds/appcds/aotFlags/AOTMergeBooleanFlag.java CONF=linux-x86_64-server-fastdebug
public class AOTMergeBooleanFlag {

    public static void main(String[] args) throws Exception {
        testFlag("-XX:+AOTMerge");
        testFlag("-XX:-AOTMerge");
        testMergeWithoutAOTCacheFails();
        testMergeWithModeFails();
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

    private static void testMergeWithoutAOTCacheFails() throws Exception {
        ProcessBuilder pb = ProcessTools.createTestJavaProcessBuilder(
                "-Xlog:aot=info",
                "-XX:+UnlockExperimentalVMOptions",
                "-XX:+AOTMerge",
                "-version");

        OutputAnalyzer out = CDSTestUtils.executeAndLog(pb, "aot-merge-no-cache");
        out.shouldContain("-XX:+AOTMerge requires -XX:AOTCache to be specified");
        out.shouldNotHaveExitValue(0);
    }

    private static void testMergeWithModeFails() throws Exception {
        ProcessBuilder pb = ProcessTools.createTestJavaProcessBuilder(
                "-XX:+UnlockExperimentalVMOptions",
                "-XX:AOTCache=dummy.aot",
                "-XX:+AOTMerge",
                "-XX:AOTMode=create",
                "-version");

        OutputAnalyzer out = CDSTestUtils.executeAndLog(pb, "aot-merge-with-mode");
        out.shouldContain("-XX:+AOTMerge cannot be used with -XX:AOTMode");
        out.shouldNotHaveExitValue(0);
    }
}
