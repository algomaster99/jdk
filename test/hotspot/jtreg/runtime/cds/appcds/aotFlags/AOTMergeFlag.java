/*
 * @test
 * @summary Test that AOTMode=merge is accepted and requires AOTCache to be specified.
 * @requires vm.cds
 * @requires vm.flagless
 * @library /test/lib /test/hotspot/jtreg/runtime/cds/appcds/test-classes
 * @build Hello
 * @run driver jdk.test.lib.helpers.ClassFileInstaller -jar hello.jar Hello
 * @run driver AOTMergeFlag
 */

import jdk.test.lib.cds.CDSTestUtils;
import jdk.test.lib.helpers.ClassFileInstaller;
import jdk.test.lib.process.OutputAnalyzer;
import jdk.test.lib.process.ProcessTools;

public class AOTMergeFlag {

    static String appJar = ClassFileInstaller.getJarPath("hello.jar");
    static String aotCacheFile = "hello.aot";
    static String helloClass = "Hello";

    public static void main(String[] args) throws Exception {
        testMergeWithCache();
        testMergeWithoutCacheFails();
    }

    private static void testMergeWithCache() throws Exception {
        ProcessBuilder pb = ProcessTools.createLimitedTestJavaProcessBuilder(
                    "-XX:AOTCacheOutput=" + aotCacheFile,
                    "-Xlog:aot",
                    "-cp", appJar, helloClass);
        OutputAnalyzer out = CDSTestUtils.executeAndLog(pb, "aot-merge-with-cache");

        pb = ProcessTools.createLimitedTestJavaProcessBuilder(
                "-Xlog:aot=info",
                "-XX:AOTMode=merge",
                "-XX:AOTCache=" + aotCacheFile,
                "-cp", appJar, helloClass);

        out = CDSTestUtils.executeAndLog(pb, "aot-merge-with-cache");
        out.shouldHaveExitValue(0);
    }

    private static void testMergeWithoutCacheFails() throws Exception {
        ProcessBuilder pb = ProcessTools.createLimitedTestJavaProcessBuilder(
                "-Xlog:aot=info",
                "-XX:AOTMode=merge",
                "-version");

        OutputAnalyzer out = CDSTestUtils.executeAndLog(pb, "aot-merge-no-cache");
        out.shouldContain("-XX:AOTMode=merge requires -XX:AOTCache to be specified");
        out.shouldNotHaveExitValue(0);
    }
}
