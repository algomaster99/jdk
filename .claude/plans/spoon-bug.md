# Bug: Fatal NoClassDefFoundError for Optional Dependencies in AOT Cache

## Symptom

Running `spoon-test.jar` with an AOT cache causes a fatal VM abort:

```
[0,209s][info][aot,load] app   org.tukaani.xz.XZInputStream (skipped: not on current classpath)
[0,209s][info][aot,load] app   org.tukaani.xz.SingleXZInputStream (skipped: not on current classpath)
[0,343s][error][aot     ] java.lang.NoClassDefFoundError: org/tukaani/xz/XZInputStream
Error occurred during initialization of VM
Unexpected exception when loading aot-linked classes.
```

The bulk loader correctly identifies and skips `XZInputStream` (not on the runtime
classpath), yet the VM aborts 130ms later with a fatal `NoClassDefFoundError` for it.

## Root Cause

### Why the class is in the cache

The AOT cache is built by Maven Surefire during `mvn package`. Surefire runs with the
**full test classpath**, which includes `org.tukaani:xz` as a transitive dependency of
`org.apache.commons:commons-compress`. The xz library is *optional* in commons-compress,
meaning it is only pulled in transitively when something else depends on it (here: the
test scope). The cache therefore records and links `org.tukaani.xz.*` classes.

When `spoon-test.jar` (the production fat JAR) is run, the xz library is **not** shaded
in, so the class is absent at runtime.

### Why skipping is not enough

The bulk loader (`AOTLinkedClassBulkLoader::load_classes_impl`) correctly skips
`XZInputStream` when it detects the class is absent:

```cpp
if (!AOTClassLocationConfig::is_class_in_current_classpath(class_name)) {
    log_info(aot, load)("%-5s %s (skipped: not on current classpath)", ...);
    continue;  // class never enters the system dictionary
}
```

However, two separate problems remain after this skip:

**Problem 1 — Pre-resolved CP entries in loaded classes.**
Classes that *are* loaded (e.g. `XZCompressorInputStream`, `LZMACompressorInputStream`)
have archived constant-pool entries that were pre-resolved at training time to point
directly at the skipped `InstanceKlass*`. At runtime the CP tag is still
`JVM_CONSTANT_Class`, so `klass_at_impl` returns the archived but never-loaded
`InstanceKlass*` bypassing the classloader entirely.

**Problem 2 — `<clinit>` execution during the init phase.**
After loading, `init_classes_for_special_subgraph` calls `ik->initialize(CHECK)` on
every class reachable from archived heap objects. Some of these classes (e.g. a
commons-compress factory class) have `<clinit>` methods that probe for available
compressor implementations. During training xz *was* available, so the probe succeeded.
At runtime the probe executes bytecode that triggers lazy CP resolution for
`XZInputStream`, which calls `resolve_or_fail` → `ClassNotFoundException` →
`NoClassDefFoundError`. Because this exception is thrown inside the init phase, it
propagates up to `exit_on_exception` and becomes fatal:

```cpp
// AOTLinkedClassBulkLoader::load_classes_in_loader, line 80-84
if (current->has_pending_exception()) {
    // "dangling C++ pointers" concern — any exception is treated as unrecoverable
    exit_on_exception(current);  // → vm_exit_during_initialization(...)
}
```

Note: the dangling-pointer concern in that comment applies to **loading** failures, not
initialization failures. A class that loaded but failed to initialize has no dangling
metadata pointers; the abort here is overly conservative for init-phase errors.

### Why commons-compress doesn't catch it

In isolation, commons-compress handles the absent xz library gracefully — its `XZUtils`
static initializer catches `NoClassDefFoundError`. But with AOT, `XZUtils` may have been
**AOT-initialized** during training (when xz was present), so its
`xzAvailable` field is already `true` in the archived mirror. At runtime
`initialize_with_aot_initialized_mirror` restores that pre-baked state without
re-running `<clinit>`, so the runtime value of `xzAvailable` is **wrong**: it says xz
is available when it is not. Downstream code then unconditionally uses xz and triggers
the fatal error through a code path that does not catch `NoClassDefFoundError`.

## Implementation Status

| Fix | Status | Description |
|-----|--------|-------------|
| Fix 1 | **TODO** | Assembly-time: unresolve stale CP entries in `FinalImageRecipes` |
| Fix 2 | **TODO** | Runtime bulk-loader: unresolve CP entries after skipping |
| Fix 3 | **TODO** | Runtime init phase: graceful `NoClassDefFoundError` handling |

Fix 2 alone is not sufficient — the `<clinit>` path (Problem 2) still aborts the VM.
Fix 3 is required to actually close the bug. Fix 1 is a correctness improvement for
merge/two-step flows.

---

## Fix Plan

### Fix 1 — Assembly-time: unresolve stale CP entries (FUTURE WORK)

**Where:** `src/hotspot/share/cds/finalImageRecipes.cpp`

**When it helps:** merge flow or two-step record→create where the assembly classpath
differs from the training classpath (e.g. fat JAR used for assembly, not for training).
Does **not** help the single-step `-XX:AOTCacheOutput` flow because training and
assembly share the same classpath, so nothing is excluded during assembly.

**What to do:** Add a private method `unresolve_cp_entries_to_excluded_classes()`,
called at the end of `load_all_classes`, and declare it in the header. The method:

1. Builds a `ResourceHashtable<Klass*, bool>` of excluded klasses — all entries in
   `_all_klasses` where `_live_klasses->at(i) == nullptr` AND the klass is a
   non-hidden `InstanceKlass` (hidden classes have `nullptr` for other reasons).
2. If the set is empty, returns immediately.
3. For each live class, iterates its preimage `ConstantPool`. For each entry tagged
   `JVM_CONSTANT_Class`, checks if `cp->resolved_klass_at(cp_idx)` is in the excluded
   set. If so:
   - `cp->tags()->at_put(cp_idx, JVM_CONSTANT_UnresolvedClass)`  — `tags()` is public;
     `tag_at_put()` is private, so use `tags()->at_put()` directly.
   - `cp->resolved_klasses()->at_put(kslot.resolved_klass_index(), nullptr)`
   - Log: `log_info(aot, load)("Unresolved CP[%d] in %s -> %s (class not on classpath)", ...)`

**Includes needed in `.cpp`:** `"utilities/resourceHash.hpp"` (not currently present).
**Header change:** add `void unresolve_cp_entries_to_excluded_classes();` to the private
section alongside `load_all_classes`.

**Key pitfall:** `tag_at_put` is `private` in `ConstantPool`. Must use the public
`tags()->at_put()` instead. `resolved_klasses()` and `klass_slot_at()` are public.

---

### Fix 2 — Runtime bulk-loader: unresolve CP entries after skipping (DONE)

**Where:** `src/hotspot/share/cds/aotLinkedClassBulkLoader.cpp`

**What to do:** In `load_classes_impl`, build a `ResourceHashtable<InstanceKlass*, bool>
skipped_classes` during the skip loop (both the hidden-class-nest-host branch and the
classpath-visibility branch). After the load loop, walk every *loaded* class's CP; for
any `JVM_CONSTANT_Class` entry whose `resolved_klass_at(cp_idx)` is in `skipped_classes`,
unresolve it in-place:
- `cp->tags()->at_put(cp_idx, JVM_CONSTANT_UnresolvedClass)` — `tags()` is public;
  `tag_at_put` is private.
- `cp->resolved_klasses()->at_put(kslot.resolved_klass_index(), nullptr)`
- Log: `log_info(aot, load)("%-5s Unresolved CP[%d] in %s -> %s (skipped class)", ...)`

The outer `ResourceMark rm(THREAD)` must be placed at the top of `load_classes_impl`
(before the loop) so the `ResourceHashtable` survives both the skip and unresolve passes.
The per-iteration `ResourceMark rm(THREAD)` calls in the existing log branches must be
removed since the outer mark now covers them.

**Includes to add:**
```cpp
#include "oops/constantPool.inline.hpp"
#include "utilities/resourceHash.hpp"
```

This fixes Problem 1 (pre-resolved CP pointers) for the single-step flow, but
**Problem 2** (`<clinit>` triggering lazy CP resolution during the init phase) still
aborts the VM — Fix 3 is required.

---

### Fix 3 — Runtime init phase: graceful NoClassDefFoundError handling (FUTURE WORK)

**Where:** `src/hotspot/share/cds/heapShared.cpp`,
`HeapShared::init_classes_for_special_subgraph` (line ~1162).

**What to do:** In pass 1 (the `do_init=true` call), replace `CHECK` with `THREAD`
and after `resolve_or_init(k, /*do_init*/true, THREAD)` check for a pending
`NoClassDefFoundError`. If found: log it and `CLEAR_PENDING_EXCEPTION` to continue
to the next class. All other exceptions should still propagate normally.

```cpp
// pass 1: initialize
resolve_or_init(k, /*do_init*/true, THREAD);
if (HAS_PENDING_EXCEPTION) {
  if (PENDING_EXCEPTION->is_a(vmClasses::NoClassDefFoundError_klass())) {
    ResourceMark rm(THREAD);
    log_info(aot, load)("Skipping initialization of %s (missing dependency: %s)",
                        k->external_name(),
                        java_lang_String::as_utf8_string(
                            java_lang_Throwable::message(PENDING_EXCEPTION)));
    CLEAR_PENDING_EXCEPTION;
    continue;
  }
  return; // propagate non-NCDFE exceptions
}
```

**Rationale:** The `exit_on_exception` comment says it exists because "dangling C++
pointers" may remain after loading failures. That concern applies to the **loading**
phase only — a class that has been fully loaded and linked (entered the system
dictionary, vtable/itable set up) but whose `<clinit>` fails leaves no dangling
metadata pointers. The failed class is placed in the standard "in error" state; any
subsequent attempt to use it throws `ExceptionInInitializerError`, catchable by the
application — the same behavior as running without AOT.

**Also needed:** Apply the same treatment to `link_class(CHECK)` in pass 0 of the same
loop, since a linking failure for a class that references a skipped class (e.g. via
supertype resolution) could fail there too.

**Includes needed:** none beyond what is already in `heapShared.cpp`
(`vmClasses.hpp` is already included; `javaClasses.hpp` for `java_lang_Throwable` is
also present).