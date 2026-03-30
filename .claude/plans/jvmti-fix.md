# Fix: SIGSEGV crashes during AOT cache creation when JVMTI agent is active

## Context

When running Apache Commons IO tests via surefire (`-XX:AOTCacheOutput=cache.aot`), **Mockito Inline with ByteBuddy** registers a JVMTI `ClassFileLoadHook` and retransforms classes at runtime. This causes TWO separate crashes during AOT cache dump.

---

## Crash 1: `record_regenerated_object()` (already fixed by user)

**Stack**: `ArchiveBuilder::record_regenerated_object+0x138` → SIGSEGV at `si_addr: 0x14`

### Root Cause
1. ByteBuddy's `ClassFileLoadHook` fires during `KlassFactory::create_from_stream()` when CDS regenerates holder classes (e.g., `DirectMethodHandle$Holder`)
2. `klassFactory.cpp:208-209`: `set_from_class_file_load_hook(result)` → regen klass **immediately excluded**
3. `lambdaFormInvokers.cpp:217`: `RegeneratedClasses::add_class(orig, result)` stores mappings for the already-excluded klass
4. `record_regenerated_objects()` → `_src_obj_table.get(regen_src_obj)` returns nullptr → crash

### Fix (already applied)
Null guard in `archiveBuilder.cpp:record_regenerated_object()`.

### Problem with this fix alone
The `_renegerated_objs` table still has entries → `has_been_regenerated()` returns true for original klass → `gather_one_source_obj()` (line 416-418) SKIPS the original. The regenerated version is excluded. **Neither version ends up in `_src_obj_table`**. Any later reference to the original crashes at `get_buffered_addr()`.

---

## Crash 2: `serialize_vm_classes()` → `get_buffered_addr()` (NEW)

**Stack**: `ArchiveBuilder::get_buffered_addr+0x48` → `SystemDictionaryShared::serialize_vm_classes+0x39` → SIGSEGV at `si_addr: 0x30`

### Root Cause
1. ByteBuddy retransforms mocked classes at runtime (e.g., `InputStream` for `InputStream$MockitoMock$...`)
2. `has_been_redefined()` flag is set on the retransformed class (e.g., `InputStream`)
3. `check_for_exclusion_impl()` (line 286) **recursively** checks supertypes: `has_been_redefined(k->java_super())`
4. Any vmClass whose supertype was retransformed is excluded (e.g., `ByteArrayInputStream` extends `InputStream`)
5. Excluded vmClass is NOT in `_klasses`, and may not be reachable from any other root → NOT in `_src_obj_table`
6. `serialize_vm_classes()` unconditionally calls `get_buffered_addr()` for ALL vmClasses → crash

### Key source locations
- `systemDictionaryShared.cpp:286-288` — `has_been_redefined()` exclusion check
- `systemDictionaryShared.cpp:609-624` — recursive `has_been_redefined()` walks supers
- `systemDictionaryShared.cpp:1145-1149` — `serialize_vm_classes()` iterates all vmClassIDs
- `archiveBuilder.cpp:772-778` — `get_buffered_addr()` asserts non-null

---

## Comprehensive Fix

### Fix A: `lambdaFormInvokers.cpp:regenerate_class()` — skip regeneration if excluded

**File: `src/hotspot/share/cds/lambdaFormInvokers.cpp` (line 210-228)**

After `KlassFactory::create_from_stream()` returns, check if the regenerated class was excluded by the JVMTI hook. If so, skip the entire regeneration — don't add to `_renegerated_objs`, don't exclude the original.

```cpp
InstanceKlass* result = KlassFactory::create_from_stream(&st,
                                                 class_name_sym,
                                                 cld,
                                                 cl_info,
                                                 CHECK);

assert(result->java_mirror() != nullptr, "must be");

// If a JVMTI ClassFileLoadHook agent modified the regenerated class bytecodes,
// KlassFactory::create_from_stream() has already excluded it. Skip regeneration
// entirely so the original class can be archived as-is (or excluded on its own merit).
DumpTimeClassInfo* regen_info = SystemDictionaryShared::dumptime_table()->get(result);
if (regen_info != nullptr && regen_info->is_excluded()) {
  log_info(aot, lambda)("Skipping regeneration of %s (excluded by JVMTI agent)", class_name);
  return;
}

RegeneratedClasses::add_class(InstanceKlass::cast(klass), result);
// ... rest unchanged
```

This prevents the original klass from being poisoned in `_renegerated_objs`, so `has_been_regenerated()` returns false and the original is gathered normally.

### Fix B: `systemDictionaryShared.cpp:serialize_vm_classes()` — handle excluded vmClasses

**File: `src/hotspot/share/classfile/systemDictionaryShared.cpp` (line 1145-1149)**

Check if each vmClass has been archived before trying to get its buffered address. Use `has_been_archived()` (archiveBuilder.cpp:757-760).

```cpp
void SystemDictionaryShared::serialize_vm_classes(SerializeClosure* soc) {
  for (auto id : EnumRange<vmClassID>{}) {
    if (soc->writing()) {
      InstanceKlass* k = *vmClasses::klass_addr_at(id);
      if (k != nullptr && !ArchiveBuilder::current()->has_been_archived((address)k)) {
        // vmClass was excluded (e.g., a JVMTI agent retransformed it or a supertype).
        // Write null — it will be loaded from classpath at runtime.
        void* null_ptr = nullptr;
        soc->do_ptr(&null_ptr);
      } else {
        soc->do_ptr(vmClasses::klass_addr_at(id));
      }
    } else {
      soc->do_ptr(vmClasses::klass_addr_at(id));
    }
  }
}
```

### Fix C: Keep null guard in `record_regenerated_object()` (defense-in-depth)

Already applied. Keep as a safety net for any edge case where Fix A doesn't cover.

---

## Existing utilities to reuse

- `ArchiveBuilder::has_been_archived()` (`archiveBuilder.cpp:757-760`) — checks if src_addr is in `_src_obj_table`
- `SystemDictionaryShared::dumptime_table()->get()` — looks up DumpTimeClassInfo
- `DumpTimeClassInfo::is_excluded()` (`dumpTimeClassInfo.hpp:201-203`)
- `RegeneratedClasses::has_been_regenerated()` (`regeneratedClasses.cpp:68-74`)

## Verification

1. `find ~/Desktop/experiments/jit-testing/pdfbox-deps -name "*.aot" -delete`
2. `make images CONF=linux-x86_64-server-release`
3. In `pdfbox-deps/apache-commons-io`: `mvn clean package` (triggers surefire with ByteBuddy agent)
4. Run `check-aot.sh` with new java image
5. Confirm no SIGSEGV at either crash point
6. With `-Xlog:aot+lambda` confirm "Skipping regeneration" messages appear for excluded holder classes
