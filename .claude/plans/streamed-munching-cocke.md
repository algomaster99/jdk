# Plan: Merge Performance Enhancements

## Context

When merging AOT caches across PDFBox Maven modules (io → fontbox → ... → tools → examples), the merged cache is **slower** than default CDS:
- Default CDS: ~1430-1598ms
- Merged AOT cache: ~2133-5173ms

The merged cache should be faster because it contains all upstream module classes.

---

## Fix 4 (CRITICAL impact): Cache ZIP handles in `is_class_in_current_classpath` ✅ IMPLEMENTED

**The real bottleneck.** At runtime, the bulk loader calls `is_class_in_current_classpath()` for every merged class whose `shared_classpath_index` doesn't match the runtime JAR path (which is ALL merged classes when running with a fat JAR like `pdfbox-app-3.0.7.jar`).

The original code **opens, parses the central directory, searches, and closes** the ZIP file on every single call. For hundreds of merged classes, this means hundreds of redundant JAR open/close cycles — each parsing the entire central directory.

**File:** `src/hotspot/share/cds/aotClassLocation.cpp`

**Change:** Added a `CachedCPEntry` struct and lazy initialization. On first call, all classpath JARs are opened and their handles cached. Subsequent calls reuse the cached `jzfile*` handles, turning each lookup into a simple `ZipLibrary::find_entry()` on an already-open file.

---

## Fix 1 (HIGH impact): Use live class pointers in `apply_recipes_for_constantpool` ✅ IMPLEMENTED

**Files:** `finalImageRecipes.hpp`, `finalImageRecipes.cpp`

In the merge flow, `load_all_classes()` resolves classes fresh from disk (`actual != ik`), but `apply_recipes_for_constantpool()` was operating on the stale preimage `ik` pointer. CP pre-resolution was wasted on dead copies.

**Change:** Added `_live_klasses` array populated during `load_all_classes()`. `apply_recipes_for_constantpool()` now uses live pointers.

---

## Fix 2 (MEDIUM impact): Fix adapter handler fingerprint hash collisions ✅ IMPLEMENTED

**Files:** `aotCodeCache.hpp`, `aotCodeCache.cpp`

Two different adapter fingerprints can hash to the same `id`. `find_entry()` only checked `kind`, returning the wrong adapter. The name mismatch caused the adapter to be discarded and regenerated at runtime.

**Change:** Extended `find_entry()` and `check_entry()` to also verify the stored blob name when a name parameter is provided. The linear scan around hash collisions now continues until it finds the entry with the matching name.

---

## Fix 3 (LOW impact): Optimize O(n*m) class deduplication in merge ✅ IMPLEMENTED

**File:** `metaspaceShared.cpp`

Replaced O(n*m) nested `strcmp` loop with a `ResourceHashtable<Symbol*>` for O(1) dedup lookups. Only affects merge build time, not runtime.

---

## Verification

1. `make images CONF=linux-x86_64-server-release`
2. Copy build output to sdkman installation
3. In pdfbox dir: `find . -type f -name "*.aot" -delete`
4. `mvn clean package`
5. `./check-aot.sh` — verify caches are correct
6. `./performance-check.sh` — compare default CDS vs AOT cache times
7. Check for adapter warnings: `java -Xlog:aot+codecache+stubs=warning -XX:AOTCache=tools/cache.aot ...`

## Possible Future Investigation

If performance is still not as expected after Fix 4, investigate:
- **Exception loops in `aotConstantPoolResolver.cpp`**: `preresolve_class_cp_entries`, `preresolve_field_and_method_cp_entries`, and `preresolve_indy_cp_entries` all have loops that throw+catch exceptions for each failed resolution. These run during assembly, affecting archive quality.
- **`load_classes_impl` exception path**: Classes that fail to load at runtime still attempt `SystemDictionaryShared::find_or_load_shared_class()` before being skipped, which could involve expensive class loader delegation.