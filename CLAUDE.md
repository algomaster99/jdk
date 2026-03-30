# Project Overview

This repository is the OpenJDK source tree: a large, multi-module Java + HotSpot VM project. It contains the JDK runtime (java.* modules), the HotSpot VM (native C/C++ code), JFR (Java Flight Recorder) internals, platform-specific implementations, and a comprehensive JTReg test-suite. Primary languages: Java and C/C++. Intended usage: build the JDK, run platform/unit/functional tests, change core runtime libraries, VM internals and JFR event/diagnostic code.

# Key Components

- src/java.base/share/classes/java/io/RandomAccessFile.java:RandomAccessFile — legacy random-access I/O with native bindings, lazy FileChannel creation and JFR tracing.
- src/java.base/share/classes/java/nio/file/FileSystems.java:FileSystems — factory/registry for FileSystemProvider and default filesystem logic.
- src/java.base/aix/classes/sun/nio/fs/AixFileSystemProvider.java:AixFileSystemProvider — AIX-specific FileSystemProvider behavior and UserDefinedFileAttributeView gating.
- src/java.base/share/classes/java/security/PEMDecoder.java:PEMDecoder — public API to decode PEM into crypto objects; wraps exceptions and supports provider/password configuration.
- src/java.base/share/classes/java/security/PEMEncoder.java:PEMEncoder — encodes DER objects to PEM; supports encryption and lazy SecretKey derivation.
- src/java.base/share/classes/java/security/PEMRecord.java:PEMRecord — public record type for generic PEM blocks (preview API).
- src/java.base/share/classes/sun/security/pkcs/PKCS8Key.java:PKCS8Key — internal PKCS#8 parsing, v1/v2 handling, secure clearing, encoding cache.
- src/java.base/share/classes/sun/security/util/Pem.java:Pem — shared PEM parsing/formatting; readPEM/decode/pbe mapping.
- src/java.base/share/classes/java/security/PEMDecoder.java and PEMEncoder.java — coordinate with PKCS8Key & Pem.
- src/java.base/share/classes/java/util/Locale.java:Locale — canonical locale factory/matching/serialization and provider lookup.
- src/java.base/share/classes/jdk/internal/javac/PreviewFeature.java:PreviewFeature — annotation used to mark preview APIs.
- src/java.base/share/classes/jdk/internal/util/HexDigits.java:HexDigits — micro-optimized hex conversion table used by HexFormat.
- src/java.base/share/classes/jdk/internal/vm/ThreadDumper.java:ThreadDumper and src/java.base/share/classes/jdk/internal/vm/ThreadSnapshot.java:ThreadSnapshot — VM-bridge for thread dumps; produces text/JSON output and must handle native null snapshots.
- src/java.base/share/classes/sun/nio/ch/NioSocketImpl.java:NioSocketImpl — socket implementation using NIO, with careful locking and virtual-thread/timeouts non-blocking transitions.

JFR (jdk.jfr) and tracing internals

- src/jdk.jfr/share/classes/jdk/jfr/internal/JVM.java:JVM — native bridge for JFR; many methods are native/intrinsic; keep signatures unchanged.
- src/jdk.jfr/share/classes/jdk/jfr/internal/MetadataRepository.java:MetadataRepository — central JFR metadata manager; serializes metadata to JVM and coordinates registrations.
- src/jdk.jfr/share/classes/jdk/jfr/internal/EventControl.java:EventControl — creates SettingControl instances and stack filters for PlatformEventType.
- src/jdk.jfr/share/classes/jdk/jfr/internal/PlatformEventType.java:PlatformEventType — per-event runtime metadata and native propagation (enable, period, thresholds).
- src/jdk.jfr/share/classes/jdk/jfr/internal/tracing/PlatformTracer.java:PlatformTracer — method tracing initialization, filters, instrumentation (onMethodTrace) and emit timing.
- src/jdk.jfr/share/classes/jdk/jfr/internal/tracing/TimedClass.java:TimedClass and TimedMethod — accumulate per-method timing aggregates and emit MethodTimingEvent.
- src/jdk.jfr/share/classes/jdk/jfr/events/MethodTimingEvent.java:MethodTimingEvent — JFR event schema (generated stubs for commit/timestamp/enabled).

HotSpot Serviceability Agent (SA) pieces

- src/jdk.hotspot.agent/share/classes/sun/jvm/hotspot/memory/FileMapInfo.java:FileMapInfo — mapping CDS/archive cloned vtable addresses to HotSpot Type.
- src/jdk.hotspot.agent/share/classes/sun/jvm/hotspot/runtime/Threads.java:Threads — SA helper to wrap VM thread list and select platform PDAccess.

Security & crypto providers

- src/java.base/share/classes/sun/security/ec/ECKeyFactory.java:ECKeyFactory — EC key conversions and zeroization.
- src/java.base/share/classes/sun/security/ec/XDHKeyFactory.java:XDHKeyFactory — X25519/X448 key factory with lockedParams.
- src/java.base/share/classes/sun/security/rsa/RSAKeyFactory.java:RSAKeyFactory — RSA/PSS key factory, size/exponent policy and zeroization.
- src/java.base/share/classes/sun/security/x509/X509Factory.java:X509Factory — CertificateFactorySpi for parsing DER/PEM/PKCS#7 and caching.

# Architecture (high-level)

    [Developer]                         [Build/Test/CI]
       │                                      │
       ▼                                      ▼
    ┌────────────────────────────┐      ┌────────────────────┐
    │  src/ (java.* modules)     │◀────▶│  make/configure     │
    │  - java.base               │      │  commands (configure,│
    │  - jdk.jfr                 │      │  make images, tests) │
    │  - jdk.hotspot.agent       │      └────────────────────┘
    └────────────────────────────┘
        ▲          ▲         ▲
        │          │         │
        │          │         │
    ┌───┴──┐   ┌───┴──┐  ┌───┴──┐
    │ HotSpot │ │ JFR  │  │ Tests │
    │ Native  │ │ Java │  │ JTReg │
    └────────┘  └──────┘  └──────┘

Data flow examples:
- File operations: java.io/RandomAccessFile ⇄ FileChannelImpl (native read/write) ⇄ native open0/read0
- JFR: Event class registration → MetadataRepository → JVM.storeMetadataDescriptor → runtime recording/commit via JVM.getEventWriter/commit
- Thread dumps: VM fills ThreadSnapshot via native create() → ThreadDumper serializes to text/JSON

# Core Data Structures (selected)

- src/java.base/share/classes/java/security/PEMRecord.java:PEMRecord(type, content, leadingData)
  - Immutable record, DEREncodable, toString delegates to Pem.pemEncoded

- src/java.base/share/classes/sun/security/pkcs/PKCS8Key.java:PKCS8Key
  - Fields: algid, privKeyMaterial, attributes, pubKeyEncoded, encodedKey (cached)
  - Important methods: decode(DerValue), generateEncoding(), getEncodedInternal(), clear() — must zero sensitive data

- src/java.base/share/classes/jdk/internal/vm/ThreadSnapshot.java:ThreadSnapshot
  - Native-populated private fields; of() returns null if native create() returned null; ordinals → enums mapping must match VM

- src/jdk.jfr/share/classes/jdk/jfr/internal/tracing/TimedMethod.java:TimedMethod
  - record with AtomicLong invocations/time, AtomicLong minimum/maximum (sentinels Long.MAX_VALUE/Long.MIN_VALUE), published flag; updateMinMax uses CAS loops and ignores duration==0

- src/jdk.jfr/share/classes/jdk/jfr/internal/util/Rate.java:Rate(amount, unit)
  - Parsing strictness: of(String) returns null on invalid input; perSecond() conversion via unit.nanos

- src/jdk.jfr/share/classes/jdk/jfr/internal/TimespanRate.java:TimespanRate
  - Represents either a rate or a timespan; OFF sentinel instance exists; selectHigherResolution merges settings.

# Control Flow (end-to-end examples)

- JFR event registration & recording:
  1. Agent registers a Java event class → MetadataRepository.register() (create EventConfiguration)
  2. MetadataRepository serializes TypeLibrary → JVM.storeMetadataDescriptor
  3. When recording starts PlatformRecorder/JVM methods beginRecording/getEventWriter are called
  4. Event emission: Event.commit → JVM.getEventWriter/commit → native writes chunk
  5. Chunk rotation/flush → CHUNK_ROTATION_MONITOR notified

- Method timing/tracing:
  1. User sets MethodSetting → EventControl defines setting
  2. MethodSetting.apply() calls PlatformTracer.ensureInitialized() then PlatformTracer.setFilters()
  3. JVM requests instrumentation via JVM upcalls → PlatformTracer.onMethodTrace modifies bytecode snippets
  4. Instrumented methods record durations → TimedMethod.updateMinMax/invocations/time
  5. Periodically PlatformTracer.emitTiming iterates TimedClass.emit → MethodTimingEvent.commit

- File attribute view on AIX:
  1. Files.getFileAttributeView calls provider.getFileAttributeView
  2. AixFileSystemProvider.supportsUserDefinedFileAttributeView(unixPath) probes FileStore via Files.getFileStore
  3. If supported returns AixUserDefinedFileAttributeView else null (swallows IOExceptions and treats as unsupported)

# Test-Driven Development

- Before coding: run targeted tests. Common examples:
  - Full build and smoke tests: bash configure && make images && ./build/*/images/jdk/bin/java -version
  - Basic JDK tests: make test-tier1
  - Run a JTReg test: jtreg -jdk:./build/<conf>/images/jdk test/jdk/…/SpecificTest.java -v
  - For JFR/tracing changes: run jfr tests under test/jdk/jdk/jfr and PlatformTracer tests (e.g., TestLazyPlatformTracer)

- When adding a test, place it under test/jdk/... and use jtreg annotations (requires vm.hasJFR, os.family, etc.)

# Bash Commands (practical)

- Configure the build: bash configure
- Build JDK images: make images
- Run basic / tier1 tests: make test-tier1
- Run specific JTReg test: jtreg -jdk:./build/<conf>/images/jdk test/jdk/path/ToTest.java -v
- Run a single test class via make: make test TEST=<testname> or make test-tier1 (see doc/testing.md)
- Quick runtime check: ./build/*/images/jdk/bin/java -version

CI: .github/workflows contains build/test workflows for linux/macos/windows/Alpine and actions for obtaining jtreg/bootjdk. Use these for reference when matching CI matrix.

# Code Style / Conventions (non-obvious)

- Native bindings: never change native method signatures or checked exceptions without coordinating native (C/C++) changes and updating registerNatives.
- Sensitive data: zero private key byte arrays after use (Arrays.fill) — many crypto classes rely on this pattern (PKCS8Key, ECKeyFactory, RSAKeyFactory).
- JFR event classes: commit/timestamp/enabled stubs are generated — do not implement or change signatures in source; metadata annotations drive codegen.
- Preview APIs: PreviewFeature annotation is retained with RetentionPolicy.CLASS; changing it affects javac/tooling compatibility.
- Concurrency: many tracing & timing classes use Atomic* and CAS loops (TimedMethod.updateMinMax) — preserve atomic semantics and sentinel init values.

# Gotchas (common pitfalls & platform caveats)

- Changing native method signatures → LinkageError/NoSuchMethodError at runtime. Fix: update native implementation and registerNatives mappings.
- PKCS#8 / PEM handling: PKCS8Key/PEMDecoder rely on internal sun.* utilities and clear sensitive arrays; removing clear() or changing caching semantics breaks security and equals/hashCode.
- JFR JVM bridge (JVM.java): many methods are native/intrinsic; do not change names, params, or checked exceptions without native updates.
- ThreadSnapshot and ThreadDumper: native VM populates fields; renaming/retyping fields breaks VM/JNI contract. of() can return null and callers must handle it.
- AixFileSystemProvider: supportsUserDefinedFileAttributeView intentionally swallows IOException and returns false — changing that changes observable Files API behavior.
- HexDigits: DIGITS ordering and short packing (low byte = low nibble ASCII, high byte = high nibble ASCII) is optimized; changing layout breaks HexFormat and callers.
- PlatformTracer.initialize(): expensive and does side-effects (module exports, instrumentation); prefer lazy initialize via MethodSetting.ensureInitialized and do not call repeatedly.
- TimedMethod/TimedClass sentinel values: MISSING = Long.MIN_VALUE used to indicate missing min/avg/max; do not alter without coordinating consumers/events.

# Pattern Examples (good examples to follow)

- src/java.base/share/classes/java/io/RandomAccessFile.java:getChannel
  - Example of thread-safe lazy init of FileChannel with double-checked locking and closed-check handling.

- src/java.base/share/classes/sun/security/pkcs/PKCS8Key.java:getEncodedInternal / clear
  - Example of synchronized caching of sensitive encoding with secure clearing and defensive cloning for external callers.

- src/jdk.jfr/share/classes/jdk/jfr/internal/tracing/TimedMethod.java:updateMinMax
  - Example of lock-free CAS updates for min/max with sentinel initial values and guard against zero-duration samples.

- src/jdk.jfr/share/classes/jdk/jfr/internal/JVM.java:registerNatives (static init)
  - Example: keep static init ordering (native registration, log tag subscription, Options.ensureInitialized()).

# Common Mistakes (symptom → likely fix)

- Symptom: LinkageError or UnsatisfiedLinkError for a JVM native method after Java edit.
  - Fix: Revert method signature change or update native code and registerNatives mapping; recompile native VM parts.

- Symptom: PEM/PKCS#8 tests fail with sensitive-byte mismatches or equals/hashCode differences.
  - Fix: Ensure PKCS8Key.decode/generateEncoding semantics preserved; do not expose internal encodedKey directly—use getEncoded() which clones.

- Symptom: JFR events missing or commit calls no-op after edits to event class.
  - Fix: Verify event annotations (Name, Label, RemoveFields) unchanged and that generated commit/timestamp stubs are rebuilt; MetadataRepository.storeDescriptorInJVM must be invoked.

- Symptom: Thread dumps produce NPE or incorrect counts.
  - Fix: ThreadSnapshot.of may return null — callers (ThreadDumper) must handle null and only count dumps when a snapshot was written.

- Symptom: Tests that use Rate/TimespanRate parsing return null or mis-parse values.
  - Fix: Confirm parsing contracts (Rate.of returns null for invalid; TimespanRate.of handles "off" and delegates to ValueParser/Rate). Check any variable-name null-check bugs.

# Invariants (must hold)

- Native method Java signatures must match native implementation and registerNatives bindings.
- Sensitive byte arrays containing private key material must be zeroed after use.
- JFR generated methods (commit/timestamp/enabled) keep their signatures and are treated as generated hooks.
- ThreadSnapshot field ordering and enum ordinals must match native VM ordinals.
- PKCS#8 encoding caching: getEncodedInternal returns internal cached bytes (synchronized) and getEncoded returns clone.

# Anti-patterns (avoid)

- Holding strong references to dynamically loaded event classes or setting classes in long-lived JFR structures — this prevents unloading.
- Replacing atomic CAS loops with non-atomic updates in hot paths (timing/tracing) — leads to lost updates or races.
- Swallowing or converting IOExceptions in security/crypto paths unexpectedly (unless deliberate and documented). Example: AixFileSystemProvider deliberately treats IO failures as 'unsupported'.

# Additional Project-Specific Notes

- Building & CI: follow doc/building.md; typical local flow: bash configure → make images → make test-tier1. CI workflows live under .github/workflows (linux/macos/windows matrix plus special Alpine/cross-compile flows).
- Tests: many are JTReg tests under test/jdk; running a single JTReg test requires jtreg and boot JDK configuration. Use the project's CI action scripts to fetch jtreg/bootjdk for repeatable execution.
- Editing JFR schema or event shapes: coordinate event annotation changes with the JFR metadata writer (MetadataDescriptor) and native consumers; do not change sentinel semantics silently (e.g., MISSING Long.MIN_VALUE).

If you tell me which specific area you will change (file or feature), I can provide a focused checklist: local tests to run, likely tests to fail, and the minimal build+test commands to validate the change.

# Verification Checklist

- Run the full test matrix locally or in CI
- Confirm failing test fails before fix, passes after
- Run linters and formatters

# Test Integrity

- NEVER modify existing tests to make your implementation pass
- If a test fails after your change, fix the implementation, not the test
- Only modify tests when explicitly asked to, or when the test itself is demonstrably incorrect

# Suggestions for Thorough Investigation

When working on a task, consider looking beyond the immediate file:
- Test files can reveal expected behavior and edge cases
- Config or constants files may define values the code depends on
- Files that are frequently changed together (coupled files) often share context

# Must-Follow Rules

1. Work in short cycles. In each cycle: choose the single highest-leverage next action, execute it, verify with the strongest available check (tests, typecheck, run, lint, or a minimal repro), then write a brief log entry of what changed + what you'll do next.
2. Prefer the smallest change that can be verified. Keep edits localized, avoid broad formatting churn, and structure work so every change is easy to revert.
3. If you're missing information (requirements, environment behavior, API contracts), do not assume. Instead: inspect code, read docs in-repo, run a targeted experiment, add temporary instrumentation, or create a minimal reproduction to learn the truth quickly.


# Index Files

I have provided an index file to help navigate this codebase:
- `.claude/docs/general_index.md`

The file is organized by directory (## headers), with each file listed as:
`- `filename` - short description. Key: `construct1`, `construct2` [CATEGORY]`

You can grep for directory names, filenames, construct names, or categories (TEST, CLI, PUBLIC_API, GENERATED, SOURCE_CODE) to quickly find relevant files without reading the entire index.

**MANDATORY RULE — NO EXCEPTIONS:** After you read, reference, or consider editing a file or folder, you MUST run:
`python .claude/docs/get_context.py <path>`

This works for **both files and folders**:
- For a file: `python .claude/docs/get_context.py <file_path>`
- For a folder: `python .claude/docs/get_context.py <folder_path>`

This is a hard requirement for EVERY file and folder you touch. Without this, you'll miss recent important information and your edit will likely fail verification. Do not skip this step. Do not assume you already know enough. Do not batch it "for later." Do not skip files even if you have obtained context about a parent directory. Run it immediately after any other action on that path.

The command returns critical context you cannot infer on your own:

**For files:**
- Edit checklist with tests to run, constants to check, and related files
- Historical insights (past bugs, fixes, lessons learned)
- Key constructs defined in the file
- Tests that exercise this file
- Related files and semantic overview
- Common pitfalls

**For folders:**
- Folder role and responsibility in the codebase
- Key files and why they matter
- Cross-cutting behaviors across the subtree
- Distilled insights from every file in that folder

**Workflow (follow this exact order every time):**
1. Identify the file or folder you need to work with.
2. Run `python .claude/docs/get_context.py <path>` and read the output.
3. Only then proceed to read, edit, or reason about it.

If you need to work with multiple paths, run the command for each one before touching any of them.

**Violations:** If you read or edit a file or folder without first running get_context.py on it, you are violating a project-level rule. Stop, run the command, and re-evaluate your changes with the new context.



---
*This knowledge base was extracted by [Codeset](https://codeset.ai) and is available via `python .claude/docs/get_context.py <file_or_folder>`*
