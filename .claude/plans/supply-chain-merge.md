# Plan: Multi-Cache AOT Merge

## Context

Currently, `AOTMode=merge` takes ONE input cache and discovers new classes by running an application. We want to support merging two (or more) pre-existing AOT caches into a combined cache, primarily for offline use (no application code needed).

**Constraint:** CDS can only load ONE static archive at startup. This is not just an API limitation (`FileMapInfo::_current_info` being a singleton) — it is fundamental to how archives are structured:

1. **Fixed-address memory layout.** An archive is a pre-serialized snapshot of metaspace objects (Klass*, Symbol*, ConstantPool*, etc.) relocated to a specific base address at build time. Two archives built independently would target the same or overlapping address ranges and cannot both be mapped into the same process.

2. **Duplicate classes.** Both archives contain JDK classes (e.g. `java/lang/String`). The JVM enforces one unique Klass per (loader, name) pair. Loading the same class from two archives would create two distinct Klass objects for the same type, breaking type identity and all `instanceof`/cast checks.

3. **No multi-archive lookup.** The entire class lookup path (`SystemDictionaryShared`, `SharedDictionary`) calls `FileMapInfo::current_info()` which returns a single archive. There is no infrastructure for searching across multiple simultaneously-loaded archives.

**Solution:** Load A as the primary archive. Fork a lightweight helper VM to extract class names from B. Load those classes in the main VM — first from the already-loaded primary archive (fast path, no disk I/O), then from the classpath for any classes not in the primary. Then run the existing merge pipeline (dedup → preimage → assembly).

---

## Syntax

```
java -XX:AOTMode=merge \
     -XX:AOTCache=a.aot \
     -XX:AOTMergeInputs=b.aot \
     -XX:AOTCacheOutput=combined.aot \
     [-cp <jars for classes unique to secondary>] \
     -version
```

- `AOTCache` = primary cache (loaded at startup, existing behavior)
- `AOTMergeInputs` = secondary cache(s) to fold in (new flag, path-separator-delimited for multiple: `b.aot:c.aot`)
- `AOTCacheOutput` = final combined output
- `-version` = needed to make the JVM start+exit without a main class (for offline merge)
- `-cp` = only needed for app classes that are in the secondary cache(s) but **absent from the primary archive**. `resolve_or_null` resolves via: (1) primary archive, (2) JDK modules, (3) classpath. A class that misses all three is silently skipped and will be absent from the combined cache. Only pass jars for modules whose unique app classes are not already in the primary.

---

## Internal Flow

```
Main VM startup:
  check_aotmode_merge():
    [EXISTING] Parse AOTCache → primary cache
    [NEW] Parse AOTMergeInputs → list of secondary cache paths
    [EXISTING] Set UseSharedSpaces=true, load primary cache

  Application runs (or -version prints and exits)

VM shutdown → start_merging_aot_cache(TRAPS):
  [EXISTING] Get archived classes from A via get_all_archived_classes()

  [NEW] Phase 0: For each secondary cache path:
    0a. Fork helper VM:
        java -XX:AOTCache=b.aot -XX:AOTDumpArchivedClassList=temp.classlist
        Helper loads B, walks B's archived dictionaries, writes class
        names to temp file, exits immediately.
    0b. Read temp classlist, load each class from the classpath via
        SystemDictionary::resolve_or_null()
    0c. Delete temp file

  [EXISTING] collect_loaded_classes_for_merge() picks up B's classes
             (they're now in the system dictionary)
  [EXISTING] Deduplication (new class wins over archived with same name)
  [EXISTING] Classpath index assignment, symbol permanence
  [EXISTING] Dump merged preimage → fork assembly child → final .aot
```

---

## File Changes

### 1. `src/hotspot/share/cds/cds_globals.hpp` — New flags

Add after `AOTCacheOutput` (line ~123):

```cpp
product(ccstr, AOTMergeInputs, nullptr,
        "Additional AOT cache files to merge (separated by path separator)")

product(ccstr, AOTDumpArchivedClassList, nullptr, DIAGNOSTIC,
        "Dump names of all archived classes to the specified file and exit")
```

### 2. `src/hotspot/share/cds/cdsConfig.hpp` — New state

```cpp
static bool _has_merge_inputs;
static GrowableArrayCHeap<const char*, mtClassShared>* _merge_input_paths;

// Accessors
static bool has_merge_inputs();
static int num_merge_inputs();
static const char* merge_input_at(int i);
```

### 3. `src/hotspot/share/cds/cdsConfig.cpp` — Parse AOTMergeInputs

**In `check_aotmode_merge()` (~line 475):** After existing setup, parse `AOTMergeInputs`:
- Split on `os::path_separator()` into `_merge_input_paths` array
- Set `_has_merge_inputs = true`

**In `check_aot_flags()` (~line 406):** Validate that `AOTMergeInputs` is only used with `AOTMode=merge`.

### 4. `src/hotspot/share/cds/metaspaceShared.cpp` — Core merge changes

**New function: `dump_archived_classlist(const char* output_path)`**
- Called in the helper VM when `AOTDumpArchivedClassList` is set
- Calls `SystemDictionaryShared::get_all_archived_classes(true, &list)`
- Writes each class's internal name to file (one per line, e.g. `java/lang/String`)
- Calls `vm_direct_exit(0)`

**New function: `fork_extract_helper(const char* cache_path, const char* classlist_path, TRAPS)`**
- Modeled on `exec_jvm_with_java_tool_options()` (line 1083)
- Builds arg list: filters out AOT flags, adds:
  - `-XX:AOTCache=<cache_path>`
  - `-XX:AOTDumpArchivedClassList=<classlist_path>`
  - `-XX:AOTMode=on`
- Forks via `CDS$ProcessLauncher::execWithJavaToolOptions()`
- Returns exit status

**New function: `load_classes_from_secondary_classlist(const char* classlist_path, TRAPS)`**
- Reads file line by line
- For each class name: `SystemDictionary::resolve_or_null(symbol, loader, CHECK)`
- Uses null loader for boot classes, system loader for app classes
- Logs warnings for classes that fail to resolve (skip them)

**New function: `extract_classes_from_secondary_caches(TRAPS)`**
- Iterates `CDSConfig::merge_input_paths()`
- For each: generates temp path `<AOTCacheOutput>.classlist.<i>`, calls `fork_extract_helper()`, then `load_classes_from_secondary_classlist()`, then deletes temp file

**Modified: `start_merging_aot_cache()` (~line 2196)**
- Insert after `AOTClassLocationConfig::dumptime_init()`, before `get_all_archived_classes()`:
  ```cpp
  if (CDSConfig::has_merge_inputs()) {
    extract_classes_from_secondary_caches(CHECK);
  }
  ```

### 5. `src/hotspot/share/cds/metaspaceShared.hpp` — Declarations

Add declarations for the new functions.

### 6. Helper VM trigger point

In early VM init (where `AOTMode=create` intercepts, or at archive load time):
- Check if `AOTDumpArchivedClassList` is set
- If so, call `dump_archived_classlist()` which writes the file and calls `vm_direct_exit(0)`
- This makes the helper VM a lightweight boot-and-exit process

### 7. `src/hotspot/share/runtime/flags/jvmFlagConstraintsRuntime.cpp`

Add constraint for `AOTMergeInputs` (validate non-empty when specified).

---

## Key Design Decisions

1. **Helper VM vs direct file parsing**: Helper VM is robust — it uses the same well-tested archive loading code. Direct parsing of archive binary format would be fragile and tightly coupled to serialization internals.

2. **`AOTMergeInputs` vs comma-separated `AOTCache`**: Separate flag avoids breaking `AOTCache` semantics for `AOTMode=on/auto`. `AOTCache` is validated as a single path in `check_aot_flags()`.

3. **Class loading from classpath (not from B's archive)**: After extracting class names from B, we load them from the classpath in the main VM. This means B's CP resolution recipes are NOT preserved — they'll be re-captured during assembly. This is acceptable: the merged cache has all classes, and the assembly VM re-resolves CPs from scratch.

4. **Scaling to N caches**: Sequential helper VM forks. Each produces a classlist, classes are loaded, then the single merge+assembly pipeline runs. Could be parallelized later.

5. **Primary cache choice is arbitrary for independently-built caches**: When all caches were built independently, each only knows about its own module's app classes. The choice of primary determines which app class is resolved from the archive (no classpath needed) vs. from the classpath — but the final combined cache is identical regardless of which cache is primary. Any of the N caches can serve as `-XX:AOTCache`; the rest go in `-XX:AOTMergeInputs`. The classpath just needs to cover the app classes of whichever caches are in `AOTMergeInputs` but not in the primary.

---

## Limitations

- Secondary caches' CP resolution state is lost (re-captured during assembly)
- All classes must be resolvable from the provided `-cp` classpath
- Requires `-version` (or similar) when no application main class is given
- Helper VM forks add startup overhead (~1 fork per secondary cache)

---

## Verification

All tests run from `~/Desktop/experiments/jit-testing/recursively-updating-aot-cache/`
with `PATH=build/linux-x86_64-server-release/images/jdk/bin:$PATH`.

### 1. Build
```
make images CONF=linux-x86_64-server-release
```

### 2. Create and combine independent caches (`orchestrate-combine.sh`)

Each module's cache is built **independently** — no module knows about the others:
```bash
java -XX:AOTCacheOutput=sub/sub.aot  -jar sub/target/sub-1.0-SNAPSHOT.jar
java -XX:AOTCacheOutput=add/add.aot  -jar add/target/add-1.0-SNAPSHOT.jar
java -XX:AOTCacheOutput=mul/mul.aot  -jar mul/target/mul-1.0-SNAPSHOT.jar
java -XX:AOTCacheOutput=math/math.aot -jar math/target/math-1.0-SNAPSHOT.jar
```

At this point each `.aot` only contains its own module's app class + whatever JDK classes
were loaded during that run. `sub.aot` has `Subtractor`; `add.aot` has `Adder`; etc.

Then combine all into one cache using `sub.aot` as the primary:
```bash
java -Xlog:aot+merge=info \
     -XX:AOTMode=merge -XX:AOTCache=sub/sub.aot \
     -XX:AOTMergeInputs="add/add.aot:mul/mul.aot:math/math.aot" \
     -XX:AOTCacheOutput=tree-combined.aot \
     -cp "add/target/add-1.0-SNAPSHOT.jar:mul/target/mul-1.0-SNAPSHOT.jar:math/target/math-1.0-SNAPSHOT.jar" \
     -version
```

**Why `-cp` is needed here:** `sub.aot` (primary) only knows about `Subtractor`.
`Adder`, `Multiplier`, and `MathApp` are each unique to their own secondary cache and are
absent from the primary archive. When `resolve_or_null` is called for those class names,
neither the primary archive nor the JDK modules can supply them — they must be loaded from
the classpath. `sub.jar` is intentionally omitted because `Subtractor` is already in the
primary and will be found there without disk I/O.

### 3. Verify combined cache (`check-aot-combine.sh`)

Runs `math.jar` with `tree-combined.aot` and checks that all four app classes
(`Subtractor`, `Adder`, `Multiplier`, `MathApp`) are loaded from the shared archive:
```bash
bash check-aot-combine.sh
```
Expected output:
```
[PASS] com.example.Subtractor (from AOT)
[PASS] com.example.Adder (from AOT)
[PASS] com.example.Multiplier (from AOT)
[PASS] com.example.MathApp (from AOT)
  Expected classes loaded from combined/tree AOT cache.
```

### 4. Verify primary cache choice is arbitrary

The same `check-aot-combine.sh` test should pass regardless of which independently-built
cache is chosen as primary. For example, using `math.aot` as primary instead of `sub.aot`:

```bash
java -Xlog:aot+merge=info \
     -XX:AOTMode=merge -XX:AOTCache=math/math.aot \
     -XX:AOTMergeInputs="sub/sub.aot:add/add.aot:mul/mul.aot" \
     -XX:AOTCacheOutput=tree-combined.aot \
     -cp "sub/target/sub-1.0-SNAPSHOT.jar:add/target/add-1.0-SNAPSHOT.jar:mul/target/mul-1.0-SNAPSHOT.jar" \
     -version
bash check-aot-combine.sh   # should still show all four PASS
```

`math.jar` is omitted from `-cp` because `MathApp` is now in the primary archive.
`sub.jar:add.jar:mul.jar` are needed because those app classes are absent from `math.aot`.
The combined cache content is identical to the one produced with `sub.aot` as primary.

### 5. Verify error handling
```bash
# Missing AOTMode=merge → error
java -XX:AOTMergeInputs=foo.aot -version
# Error: -XX:AOTMergeInputs requires -XX:AOTMode=merge

# Missing AOTCache → error
java -XX:AOTMode=merge -version
# Error: -XX:AOTMode=merge requires -XX:AOTCache to be specified
```
