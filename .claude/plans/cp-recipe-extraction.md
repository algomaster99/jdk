# Plan C: Extract CP Recipes from Secondary Caches During Merge

## Context

When merging AOT caches, secondary cache classes are loaded fresh from classpath — their constant pool entries start unresolved. Even if primary cache classes retain their pre-resolved CP state, secondary classes contribute zero training data. This means the merged cache has degraded CP resolution coverage compared to caches that were trained with real workloads.

**Goal**: Have the helper VM extract CP resolution recipes from secondary caches and replay them in the merge VM, so the merged cache preserves training data from ALL input caches without needing to re-run their workloads.

## How CP Recipes Work Today

The existing two-phase pipeline already captures and replays CP recipes:

1. **Phase 1 (preimage)**: `FinalImageRecipes::record_recipes_for_constantpool()` walks all loaded classes, records which CP indices are resolved (class refs, field/method refs, indy entries), and stores them in the preimage.

2. **Phase 2 (assembly)**: `FinalImageRecipes::apply_recipes_for_constantpool()` replays those resolutions using `AOTConstantPoolResolver::preresolve_*` functions.

The preresolve functions are already callable in Phase 1 — `classListParser.cpp` does this during normal training. They safely skip entries that can't be resolved.

## Design

### Enhanced Classlist Format

Currently the helper VM dumps one class name per line:
```
java/lang/String
com/example/Foo
```

New format adds CP recipe data after a tab separator:
```
java/lang/String
com/example/Foo	7	74,75,82,12,15,20,25
```

Where:
- `7` = flags bitmask (HAS_CLASS=1 | HAS_FIELD_AND_METHOD=2 | HAS_INDY=4)
- `74,75,...` = comma-separated CP indices that were resolved in the secondary cache

Classes with no resolved CP entries use the plain format (backward compatible).

### File Changes

#### 1. `src/hotspot/share/cds/metaspaceShared.cpp`

**Modify `dump_archived_classlist()`** (~line 333):

Currently writes just class names. Add CP recipe extraction logic (similar to `FinalImageRecipes::record_recipes_for_constantpool()`):

```cpp
static void dump_archived_classlist(const char* output_path) {
  fileStream fs(output_path);
  // ... existing open check ...

  ResourceMark rm;
  GrowableArray<Klass*> classes;
  SystemDictionaryShared::get_all_archived_classes(true, &classes);

  for (int i = 0; i < classes.length(); i++) {
    Klass* k = classes.at(i);
    if (!k->is_instance_klass()) continue;
    InstanceKlass* ik = InstanceKlass::cast(k);

    // Write class name
    fs.print("%s", ik->name()->as_C_string());

    // Extract CP recipes (same logic as record_recipes_for_constantpool)
    ConstantPool* cp = ik->constants();
    ConstantPoolCache* cp_cache = cp->cache();
    GrowableArray<int> cp_indices;
    int flags = 0;

    // Walk CP for resolved class refs
    for (int ci = 1; ci < cp->length(); ci++) {
      if (cp->tag_at(ci).value() == JVM_CONSTANT_Class) {
        Klass* ref = cp->resolved_klass_at(ci);
        if (ref != nullptr && ref->is_instance_klass()) {
          cp_indices.append(ci);
          flags |= 0x1; // HAS_CLASS
        }
      }
    }

    // Walk CP cache for resolved field/method/indy entries
    if (cp_cache != nullptr) {
      // ... field entries (flags |= 0x2) ...
      // ... method entries (flags |= 0x2) ...
      // ... indy entries (flags |= 0x4) ...
    }

    // Write recipe data if any
    if (cp_indices.length() > 0) {
      fs.print("\t%d\t", flags);
      for (int j = 0; j < cp_indices.length(); j++) {
        if (j > 0) fs.print(",");
        fs.print("%d", cp_indices.at(j));
      }
    }
    fs.print_cr("");
  }
  vm_direct_exit(0);
}
```

**Modify `load_and_link_classes_from_secondary_classlist()`** (~line 2279):

After loading and linking each class, parse the recipe data and call preresolve functions:

```cpp
static void load_and_link_classes_from_secondary_classlist(const char* classlist_path, TRAPS) {
  // Phase 1: Load and link all classes (existing code)
  // Store class → recipe mapping
  GrowableArray<InstanceKlass*> loaded_classes;
  GrowableArray<int> recipe_flags;
  GrowableArray<GrowableArray<int>*> recipe_indices;

  // ... parse lines, load classes, store recipes ...

  // Phase 2: Replay CP resolutions for loaded classes
  for (int i = 0; i < loaded_classes.length(); i++) {
    InstanceKlass* ik = loaded_classes.at(i);
    if (ik == nullptr) continue;
    int flags = recipe_flags.at(i);
    GrowableArray<int>* indices = recipe_indices.at(i);
    if (indices == nullptr || indices->length() == 0) continue;

    ConstantPool* cp = ik->constants();
    GrowableArray<bool> preresolve_list(cp->length(), cp->length(), false);
    for (int j = 0; j < indices->length(); j++) {
      int idx = indices->at(j);
      if (idx > 0 && idx < cp->length()) {
        preresolve_list.at_put(idx, true);
      }
    }

    if (flags & 0x1) { // HAS_CLASS
      AOTConstantPoolResolver::preresolve_class_cp_entries(THREAD, ik, &preresolve_list);
    }
    if (flags & 0x2) { // HAS_FIELD_AND_METHOD
      AOTConstantPoolResolver::preresolve_field_and_method_cp_entries(THREAD, ik, &preresolve_list);
    }
    if (flags & 0x4) { // HAS_INDY
      AOTConstantPoolResolver::preresolve_indy_cp_entries(THREAD, ik, &preresolve_list);
    }
    if (HAS_PENDING_EXCEPTION) CLEAR_PENDING_EXCEPTION;
  }
}
```

#### 2. `src/hotspot/share/cds/aotConstantPoolResolver.hpp`

No changes needed — the preresolve functions are already public.

### Key Assumptions & Risks

1. **Preresolve in Phase 1 is safe**: `classListParser.cpp` already calls these functions during Phase 1. They silently skip unresolvable entries.

2. **Target classes must be loaded**: CP resolution requires the referenced classes to be in the system dictionary. Since we load ALL secondary classes before replaying recipes, most targets should be available. Classes from the primary cache are also available.

3. **Indy resolution requires `is_dumping_invokedynamic()`**: This requires `is_dumping_aot_linked_classes() && is_dumping_heap()`. In merge mode, heap dumping is disabled in Phase 1 (preimage), so **indy recipes may NOT be replayable in Phase 1**. This is a limitation — indy entries from secondary caches would be lost. Could be addressed by deferring indy replay to Phase 2.

4. **Order matters**: All classes should be loaded and linked before replaying any recipes, since class A's CP may reference class B which was also in the secondary cache.

### Limitations

- Indy CP entries from secondary caches may not be replayable in Phase 1 (requires heap dumping context)
- CP recipes are only as good as the original training run of each secondary cache
- If a target class from a secondary cache's CP reference isn't on the merge classpath, that resolution is silently skipped

### Verification

1. Build: `make images CONF=linux-x86_64-server-release`
2. Run merge with `-version` (no workload): `bash orchestrate-combine-3.sh`
3. Run benchmarks: `bash performance-check-tree.sh`
4. Compare tree.aot vs single.aot — tree.aot should now benefit from prior training in each input cache
5. Add logging (`-Xlog:aot+merge=debug`) to verify CP recipes are being extracted and replayed