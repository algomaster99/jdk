/*
 * @test
 * @summary Test that AOTMode=merge is accepted and requires AOTCache to be specified.
 * @library /test/lib
 * @run driver AOTMergeFlag
 */

import jdk.test.lib.cds.CDSTestUtils;
import jdk.test.lib.process.OutputAnalyzer;
import jdk.test.lib.process.ProcessTools;

// Execute with
// make run-test TEST=runtime/cds/appcds/aotFlags/AOTMergeFlag.java CONF=linux-x86_64-server-fastdebug
public class AOTMergeFlag {

    public static void main(String[] args) throws Exception {
        testMergeWithCache();
        testMergeWithoutCacheFails();
    }

    private static void testMergeWithCache() throws Exception {
        ProcessBuilder pb = ProcessTools.createTestJavaProcessBuilder(
                "-Xlog:aot=info",
                "-XX:AOTMode=merge",
                "-XX:AOTCache=dummy.aot",
                "-version");

        OutputAnalyzer out = CDSTestUtils.executeAndLog(pb, "aot-merge-with-cache");
        out.shouldContain("AOT cache merge enabled with AOTCache=dummy.aot");
        out.shouldHaveExitValue(0);
    }

    private static void testMergeWithoutCacheFails() throws Exception {
        ProcessBuilder pb = ProcessTools.createTestJavaProcessBuilder(
                "-Xlog:aot=info",
                "-XX:AOTMode=merge",
                "-version");

        OutputAnalyzer out = CDSTestUtils.executeAndLog(pb, "aot-merge-no-cache");
        out.shouldContain("-XX:AOTMode=merge requires -XX:AOTCache to be specified");
        out.shouldNotHaveExitValue(0);
    }
}
