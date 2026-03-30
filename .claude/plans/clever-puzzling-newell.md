# Plan: Enable Heap Region (hp) Dumping in AOT Cache Merge Mode

## How is the heao dumped in the first place?

<Find it out and write here>.

## Context

The AOT cache merge mode (`-XX:AOTMode=merge`) currently produces archives with **no heap region**. Heap dumping is explicitly disabled via `CDSConfig::disable_heap_dumping()` in `start_merging_aot_cache()`. This means the merged archive lacks pre-archived Java mirrors, interned strings, Integer/Long/Byte/Short/Character caches, resolved references, and other heap objects that a normal CDS archive includes. This causes the merged archive to perform **worse** than a standard CDS archive for pdfbox and other projects.

**Goal:** Enable heap dumping in merge mode so the merged archive includes the hp region with archived Java objects, matching or exceeding normal CDS performance.

**Key constraint:** Only archive heap objects for classes that were **loaded** during the merge run. Unloaded shared classes (null CLD) from the old archive are naturally skipped since they lack scratch mirrors.

---

## Changes (6 files)

### 1. `src/hotspot/share/cds/cdsConfig.cpp`

**A. `is_dumping_heap()` (line 908):** Add `is_merging_aot_cache()` to the gate.

```cpp
bool CDSConfig::is_dumping_heap() {
  if (!(is_dumping_classic_static_archive() || is_dumping_final_static_archive() || is_merging_aot_cache())
      || are_vm_options_incompatible_with_dumping_heap()
      || _disable_heap_dumping) {
    return false;
  }
  return true;
}
```

**B. `allow_only_single_java_thread()` (line 784):** Add merge mode (at VM shutdown, only one Java thread is active).

```cpp
bool CDSConfig::allow_only_single_java_thread() {
  return is_dumping_classic_static_archive() || is_dumping_final_static_archive() || is_merging_aot_cache();
}
```

### 2. `src/hotspot/share/cds/metaspaceShared.cpp` — `start_merging_aot_cache()`

**A. Remove `disable_heap_dumping()` call (line 2167).** Delete this line:
```cpp
CDSConfig::disable_heap_dumping();  // REMOVE
```

**B. Add heap initialization after `AOTClassLinker::initialize()` (after line 2175) and before the class collection code.** This mirrors the heap setup from `preload_and_dump()` (lines 957-1023):

```cpp
#if INCLUDE_CDS_JAVA_HEAP
if (CDSConfig::is_dumping_heap()) {
  // Basic type scratch mirrors were not created at VM boot (is_dumping_heap was false then).
  HeapShared::init_scratch_objects_for_basic_type_mirrors(CHECK);

  // Create scratch mirrors retroactively for all loaded classes.
  // In normal mode, scratch mirrors are created during class loading when is_dumping_heap() is true.
  // In merge mode, classes were loaded before heap dumping was enabled, so we create them now.
  HeapShared::create_scratch_mirrors_for_merge(current, CHECK);

  HeapShared::init_for_dumping(CHECK);
}
#endif
```

**C. Add heap finalization before the dump operation (before line 2290, after class collection and dumptime_info init):**

```cpp
#if INCLUDE_CDS_JAVA_HEAP
if (CDSConfig::is_dumping_heap()) {
  ArchiveHeapWriter::init();

  if (CDSConfig::is_dumping_full_module_graph()) {
    ClassLoaderDataShared::ensure_module_entry_tables_exist();
    HeapShared::reset_archived_object_states(CHECK);
  }

  AOTReferenceObjSupport::initialize(CHECK);
  AOTReferenceObjSupport::stabilize_cached_reference_objects(CHECK);

  if (CDSConfig::is_initing_classes_at_dump_time()) {
    log_debug(aot)("Resetting Class::reflectionFactory");
    TempNewSymbol method_name = SymbolTable::new_symbol("resetArchivedStates");
    Symbol* method_sig = vmSymbols::void_method_signature();
    JavaValue result(T_VOID);
    JavaCalls::call_static(&result, vmClasses::Class_klass(),
                           method_name, method_sig, CHECK);
  }

  StringTable::allocate_shared_strings_array(CHECK);
} else {
  CDSConfig::stop_using_optimized_module_handling();
}
#endif
```

### 3. `src/hotspot/share/cds/heapShared.cpp`

**A. New function `create_scratch_mirrors_for_merge()`:**

Iterate all loaded classes and create scratch mirrors retroactively. Insert near `init_for_dumping()` (around line 2022).

```cpp
void HeapShared::create_scratch_mirrors_for_merge(JavaThread* current, TRAPS) {
  assert(CDSConfig::is_merging_aot_cache(), "only for merge mode");
  assert(_scratch_objects_table != nullptr, "init_dumping must be called first");

  // Create scratch mirrors for all loaded classes.
  // In normal CDS dumps, this happens during class loading (javaClasses.cpp:1154).
  // In merge mode, classes were loaded before is_dumping_heap() became true.
  ClassLoaderDataGraph::classes_do([&](Klass* k) {
    if (k->is_instance_klass()) {
      java_lang_Class::create_scratch_mirror(k, CHECK);
    } else if (k->is_array_klass()) {
      java_lang_Class::create_scratch_mirror(k, CHECK);
    }
  });
}
```

Note: `create_scratch_mirror()` (javaClasses.cpp:1176) only archives mirrors for boot/platform/app loader classes (returns early for custom loaders). This is fine — custom loader class mirrors are not archived in normal mode either.

**B. Adjust `serialize_tables()` (line 1051-1056):** The reset is only needed when heap is NOT dumped. With heap dumped, `write_heap()` populates these tables fresh.

```cpp
if (soc->writing() && CDSConfig::is_merging_aot_cache() && !CDSConfig::is_dumping_heap()) {
    _run_time_subgraph_info_table.reset();
    _run_time_special_subgraph = nullptr;
}
```

### 4. `src/hotspot/share/cds/heapShared.hpp`

**Add declaration** (near line 442):

```cpp
static void create_scratch_mirrors_for_merge(JavaThread* current, TRAPS) NOT_CDS_JAVA_HEAP_RETURN;
```

### 5. `src/hotspot/share/classfile/stringTable.cpp`

**Adjust `serialize_shared_table_header()` (line 1116-1118):** Same logic — only reset when heap is NOT dumped.

```cpp
if (soc->writing() && CDSConfig::is_merging_aot_cache() && !CDSConfig::is_dumping_heap()) {
    _shared_table.reset();
}
```

### 6. `src/hotspot/share/cds/heapShared.cpp` — `serialize_tables()` test class reset

The `_archived_ArchiveHeapTestClass` reset (line 1044) should also be guarded:

```cpp
#ifndef PRODUCT
  if (soc->writing() && CDSConfig::is_merging_aot_cache() && !CDSConfig::is_dumping_heap()) {
    _archived_ArchiveHeapTestClass = nullptr;
  }
#endif
```

---

## How Unloaded Shared Classes Are Handled

Classes from the old archive that were never loaded during the merge run:
- **Metadata**: Still included in the merged archive (existing merge logic handles this)
- **Heap (mirrors)**: Naturally skipped. `scan_java_mirror()` (heapShared.cpp:691-696) calls `scratch_java_mirror()` which returns null for these classes (no scratch mirror was created). It returns early.
- **At runtime**: These classes create mirrors normally when first loaded (standard CDS behavior for classes without archived mirrors)

---

## Ordering in `start_merging_aot_cache()`

```
1.  enable_dumping_static_archive()           // existing
2.  // disable_heap_dumping() REMOVED
3.  AOTClassLocationConfig::dumptime_init()    // existing
4.  SystemDictionaryShared::initialize()       // existing — now calls init_dumping() since is_dumping_heap()=true
5.  AOTClassLinker::initialize()               // existing
6.  NEW: init_scratch_objects_for_basic_type_mirrors()
7.  NEW: create_scratch_mirrors_for_merge()
8.  NEW: init_for_dumping()
9.  (existing class collection: get_all_archived_classes, collect_loaded_classes, assign indices, etc.)
10. (existing dumptime_info init loop)
11. NEW: ArchiveHeapWriter::init() + module graph + reference obj support + string array
12. DumperThreadMark + StaticArchiveBuilder + VM_PopulateDumpSharedSpace  // existing
```

---

## Verification

1. **Build:** `make images CONF=linux-x86_64-server-release`

2. **Basic test (recursively-updating-aot-cache):**
   ```bash
   cd ~/Desktop/experiments/jit-testing/
   # Run orchestrate.sh and check-aot.sh with new java
   ```

3. **PDFBox test:**
   ```bash
   cd <pdfbox-dir>
   find . -type f -name "*.aot" -delete
   mvn clean package
   # Run check-aot.sh
   # Run performance-check.sh
   ```

4. **Verify hp region is populated:**
   ```bash
   java -jar ~/Desktop/chains/aotp/aotp/target/aotp-0.0.1-SNAPSHOT.jar combined.aot --header
   # Should show non-zero hp region
   ```

---

## Current Limitation: Subgraph Archiving Skipped

In the current implementation, `archive_subgraphs()` is skipped in merge mode because
the VM has been running the application — subgraph root objects (e.g., `ArchivedModuleGraph`,
`BaseLocale`, boxed primitive caches) contain stale runtime-specific values that cause
`ExceptionInInitializerError` when loaded by a fresh VM.

The merged archive still includes class mirrors, interned strings, and resolved references,
which provide the main performance benefit. But it misses the boxed primitive caches and
other subgraph objects that a normal CDS archive includes.

---

## Alternative Approach: Fresh-VM Assembly for Merge

Instead of merging at VM shutdown (where the heap is contaminated), use a **two-phase
approach** that mirrors how normal training + assembly already works:

1. **Training run (merge-mode VM):** Run the app with `-XX:AOTMode=merge`. At shutdown,
   instead of dumping a full archive with heap, only produce metadata:
   - The merged class list (old archive classes + newly loaded classes)
   - Classpath information
   - Any other metadata needed for assembly

2. **Assembly run (fresh VM):** Launch a **new, clean VM** in assembly mode
   (`-XX:AOTMode=create` or a new `-XX:AOTMode=assemble`) that:
   - Reads the merged class list from step 1
   - Loads all the classes in a clean environment
   - Creates scratch mirrors, subgraph objects, module graph, etc. from a pristine state
   - Dumps the full archive including a clean heap region

This approach:
- **Solves the stale heap problem** completely — the assembly VM has never run app code,
  so all heap objects (subgraphs, module graph, etc.) are clean
- **Mirrors existing CDS workflow** — the standard `AOTMode=record` → `AOTMode=create`
  pipeline already separates training from assembly
- **Enables full module graph and AOT-linked classes** in the merged archive, since the
  assembly VM's module graph is pristine
- **Enables `is_initing_classes_at_dump_time()`** which unlocks method handle archiving,
  invokedynamic archiving, and other advanced optimizations

### Implementation sketch

```
# Phase 1: Training run (produces metadata only)
java -XX:AOTMode=merge \
     -XX:AOTCache=old.aot \
     -XX:AOTCacheOutput=merged,aot,config \
     -jar app.jar

# Phase 2: Assembly run (fresh VM, produces full archive with heap)
java -XX:AOTMode=create \
     -XX:AOTConfiguration=merged.aot.config \
     -XX:AOTCacheOutput=merged.aot
```

This would require:
- A way to export the merged aot config (old + new classes) from the training run
- The assembly VM to understand how to load classes from a previous archive + new classpath
- Possibly a new AOTMode or flag to indicate "assemble from merged aot config"

This is a more significant change but would produce archives that are fully equivalent to
normal CDS archives in quality (full heap, module graph, AOT-linked classes).

### Verification check

After implementing, use the pdfbox project to verify that the merged archive has a populated hp region and that performance matches or exceeds normal CDS. Also verify that subgraph objects (e.g., boxed primitive caches) are present in the merged archive, which would confirm that the heap is being dumped correctly.

/home/aman/Desktop/experiments/jit-testing/pdfbox

1. Delete all *.aot files in the pdfbox project
2. Run mvn clean package with the newly built java image
3. Make sure you have multiple *.aot file but not *.aot.config files
4. You can use "java -jar ~/Desktop/chains/aotp/aotp/target/aotp-0.0.1-SNAPSHOT.jar <cache.aot> --header"
   to print the header and see "used" attribute is non-zero for hp region.