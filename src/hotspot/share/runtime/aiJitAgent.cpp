/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifdef COMPILER2

#include "classfile/systemDictionary.hpp"
#include "code/codeCache.hpp"
#include "code/debugInfoRec.hpp"
#include "code/dependencies.hpp"
#include "code/nmethod.hpp"
#include "code/scopeDesc.hpp"
#include "compiler/compileBroker.hpp"
#include "compiler/compilerDefinitions.inline.hpp"
#include "compiler/directivesParser.hpp"
#include "interpreter/bytecode.hpp"
#include "interpreter/bytecodeStream.hpp"
#include "logging/log.hpp"
#include "memory/resourceArea.hpp"
#include "oops/method.inline.hpp"
#include "oops/methodData.hpp"
#include "prims/methodHandles.hpp"
#include "runtime/aiJitAgent.hpp"
#include "runtime/deoptimization.hpp"
#include "runtime/handles.inline.hpp"
#include "runtime/hotCodeSampler.hpp"
#include "runtime/interfaceSupport.inline.hpp"
#include "runtime/java.hpp"
#include "runtime/javaThread.inline.hpp"
#include "runtime/mutexLocker.hpp"
#include "runtime/os.hpp"
#include "runtime/safepointVerifiers.hpp"
#include "utilities/ostream.hpp"

// Bounds for 'unroll' hints. The default LoopUnrollLimit on x86 is 60.
static const intx MinUnrollHint = 8;
static const intx MaxUnrollHint = 600;

// One accepted hint line.
class AIJitHint : public CHeapObj<mtCompiler> {
 public:
  static const int MaxNameLength = 256;

  bool _is_inline;
  char _method[MaxNameLength];
  char _arg[MaxNameLength];   // callee for 'inline', limit for 'unroll'
};

GrowableArrayCHeap<AIJitHint*, mtCompiler>* AIJitAgentThread::_hints = nullptr;

// "pkg.Class::name", the form used in the profile, in hints and in directives.
static const char* name_of(Method* m) {
  stringStream ss;
  ss.print("%s::%s", m->method_holder()->external_name(), m->name()->as_C_string());
  return ss.as_string();
}

// A hot C2-compiled method as seen in one round.
class HotMethod : public ResourceObj {
 public:
  Method*     _method;
  Handle      _mirror;      // keeps the holder, and so _method, alive across safepoints
  const char* _name;
  int         _samples;
  int         _code_bytes;
  bool        _osr;
  GrowableArray<Method*>     _inlined;   // methods C2 inlined into this method
  GrowableArray<const char*> _callees;   // callee names listed in the profile

  HotMethod(JavaThread* thread, Method* m) :
    _method(m), _mirror(thread, m->method_holder()->java_mirror()), _name(name_of(m)),
    _samples(0), _code_bytes(0), _osr(false) {}

  bool calls(const char* callee) const {
    for (int i = 0; i < _callees.length(); i++) {
      if (strcmp(_callees.at(i), callee) == 0) {
        return true;
      }
    }
    return false;
  }
};

void AIJitAgentThread::initialize() {
  EXCEPTION_MARK;

  if (AIJitAgentCommand == nullptr || !CompilerConfig::is_c2_enabled() || !SegmentedCodeCache) {
    warning("AIJitAgent requires C2, SegmentedCodeCache and AIJitAgentCommand; agent disabled");
    return;
  }
  _hints = new GrowableArrayCHeap<AIJitHint*, mtCompiler>();

  Handle thread_oop = JavaThread::create_system_thread_object("AIJitAgentThread", CHECK);
  AIJitAgentThread* thread = new AIJitAgentThread();
  JavaThread::vm_exit_on_osthread_failure(thread);
  JavaThread::start_internal_daemon(THREAD, thread, thread_oop, NormPriority);

  log_info(aijit)("Agent thread started, command: %s", AIJitAgentCommand);
}

void AIJitAgentThread::thread_entry(JavaThread* thread, TRAPS) {
  for (uint round = 1; round <= AIJitAgentRounds; round++) {
    thread->sleep(AIJitAgentIntervalMillis);

    ResourceMark rm(thread);
    HandleMark hm(thread);
    run_round(thread, round, THREAD);
    if (HAS_PENDING_EXCEPTION) {
      CLEAR_PENDING_EXCEPTION;
    }
  }
  log_info(aijit)("Agent thread done after %u rounds", AIJitAgentRounds);
}

// One round: sample hot code, ask the agent for hints, apply them. TRAPS adds the THREAD parameter used for exceptions.
void AIJitAgentThread::run_round(JavaThread* thread, uint round, TRAPS) {
  // Empty table of compile_id -> (nmethod, sample count), filled by the loop below.
  ThreadSampler sampler;
  jlong start = os::javaTimeMillis();
  // Keep sampling until AIJitAgentSampleMillis (default 1000 ms) have passed.
  while (os::javaTimeMillis() - start <= (jlong)AIJitAgentSampleMillis) {
    if (!sampler.sample_all_java_threads()) {
      // Exclude threads such as gc workers, c1/c2 compiler threads, our own AIJitAgentThread
      // Deoptimization thread, thread running this JVM
      break;
    }
    // Wait a random 5-15 ms (HotCodeMin/MaxSamplingMs) so passes don't line up with periodic work.
    thread->sleep(rand_sampling_period_ms());
  }
  // Copy the table into a list with one entry per sampled nmethod.
  Candidates candidates(sampler);

  // The hot methods for this round; freed by the ResourceMark in thread_entry.
  GrowableArray<HotMethod*> hot;
  // Keep the hottest still-valid C2 methods (at most AIJitAgentMaxMethods) and record what C2 inlined into each.
  collect_hot_methods(thread, candidates, &hot);
  // No sample landed in usable C2 code, e.g. the program is still warming up.
  if (hot.is_empty()) {
    // Log it (visible with -Xlog:aijit).
    log_info(aijit)("Round %u: no hot C2 methods sampled", round);
    // Skip the agent this round.
    return;
  }

  // In-memory string buffer for the profile file name.
  stringStream profile_path;
  // e.g. /tmp/aijit-<pid>-<round>-profile.json
  profile_path.print("%s/aijit-%d-%u-profile.json", os::get_temp_directory(), os::current_process_id(), round);
  // Buffer for the name of the file the agent writes its hints to.
  stringStream hints_path;
  // e.g. /tmp/aijit-<pid>-<round>-hints.txt
  hints_path.print("%s/aijit-%d-%u-hints.txt", os::get_temp_directory(), os::current_process_id(), round);
  // Scope so the file is closed before the agent reads it.
  {
    // Open (and truncate) the profile file for writing.
    fileStream out(profile_path.base(), "w");
    // Opening failed, e.g. the temp directory is not writable.
    if (!out.is_open()) {
      // Log a warning with the path.
      log_warning(aijit)("Round %u: cannot write %s", round, profile_path.base());
      // Skip this round.
      return;
    }
    // Write the JSON; total_samples is the number of samples in non-profiled (C2 and tier-1) code.
    write_profile(thread, &out, round, sampler.non_profiled_sample_count(), &hot);
  }  // out's destructor flushes and closes the file here.

  // Buffer for the shell command line.
  stringStream cmd;
  // "<AIJitAgentCommand> <profile.json> <hints.txt>"
  cmd.print("%s %s %s", AIJitAgentCommand, profile_path.base(), hints_path.base());
  // Log the round, the number of hot methods and the command.
  log_info(aijit)("Round %u: %d hot methods, running: %s", round, hot.length(), cmd.base());

  // Start time, to report how long the agent took.
  jlong agent_start = os::javaTimeMillis();
  // The agent's exit status.
  int status;
  // Scope that limits the state change below to the agent call.
  {
    // The agent may take seconds; let safepoints proceed meanwhile.
    // Switch this thread from "in VM" to "in native", so safepoints and GC stop waiting for it.
    ThreadToNativeFromVM ttn(thread);
    // Run "sh -c <cmd>" as a child process and wait for it to exit; only this thread waits.
    status = os::fork_and_exec(cmd.base());
  }  // ttn's destructor switches back to "in VM", waiting here if a safepoint is in progress.
  // Log the exit status and how long the agent took.
  log_info(aijit)("Round %u: agent exited with status %d after " JLONG_FORMAT " ms",
                  round, status, os::javaTimeMillis() - agent_start);
  // A non-zero status means the agent failed, so its hints file is ignored.
  if (status != 0) {
    // Skip the rest of this round.
    return;
  }

  // Hot methods that get at least one accepted hint.
  GrowableArray<HotMethod*> changed;
  // Parse the hints file, check each hint against this round's profile, keep the valid ones.
  read_hints(hints_path.base(), &hot, &changed);
  // The agent sent no hints, or all of them were rejected.
  if (changed.is_empty()) {
    // Log it.
    log_info(aijit)("Round %u: no hints accepted", round);
    // Nothing to apply.
    return;
  }
  // Turn all hints so far for these methods into compiler directives and install them.
  install_directives(&changed);
  // For each method that got a new hint...
  for (int i = 0; i < changed.length(); i++) {
    // Throw away its current C2 code and queue a tier-4 compile that uses the new directive; CHECK returns if an exception is pending.
    recompile(changed.at(i), CHECK);
  }
}

void AIJitAgentThread::collect_hot_methods(JavaThread* thread, Candidates& candidates,
                                           GrowableArray<HotMethod*>* hot) {
  candidates.sort();   // pop() returns the hottest
  while (candidates.has_candidates() && hot->length() < (int)AIJitAgentMaxMethods) {
    Candidate c = candidates.get_candidate();

    // The sampled nmethod may be gone by now; revalidate as HotCodeCollector does.
    MutexLocker ml(CodeCache_lock, Mutex::_no_safepoint_check_flag);
    CodeBlob* cb = CodeCache::find_blob(c.get_nmethod());
    nmethod* nm = (cb == nullptr) ? nullptr : cb->as_nmethod_or_null();
    if (nm == nullptr || nm != c.get_nmethod() || nm->compile_id() != c.get_compile_id() ||
        !nm->is_in_use() || !nm->is_compiled_by_c2() || nm->method() == nullptr ||
        nm->method()->method_holder()->is_hidden()) {
      continue;
    }
    HotMethod* hm = nullptr;
    for (int i = 0; i < hot->length() && hm == nullptr; i++) {
      if (hot->at(i)->_method == nm->method()) {
        hm = hot->at(i);
      }
    }
    if (hm == nullptr) {
      hm = new HotMethod(thread, nm->method());
      hot->append(hm);
    }
    hm->_samples += c.get_sample_count();
    hm->_code_bytes = MAX2(hm->_code_bytes, nm->insts_size());
    hm->_osr |= nm->is_osr_method();

    // Inlined methods: with AlwaysRecordEvolDependencies (the default) C2 records
    // an evol_method dependency for every method it parses. Non-root scopes in
    // the debug info are a fallback, but miss inlinees without debug info points
    // such as trivial accessors.
    for (Dependencies::DepStream deps(nm); deps.next(); ) {
      if (deps.type() == Dependencies::evol_method && deps.method_argument(0) != nm->method()) {
        hm->_inlined.append_if_missing(deps.method_argument(0));
      }
    }
    for (PcDesc* p = nm->scopes_pcs_begin(); p < nm->scopes_pcs_end(); p++) {
      if (p->scope_decode_offset() == DebugInformationRecorder::serialized_null) {
        continue;
      }
      for (ScopeDesc* sd = nm->scope_desc_at(p->real_pc(nm)); !sd->is_top(); sd = sd->sender()) {
        hm->_inlined.append_if_missing(sd->method());
      }
    }
  }
}

// Total profiled count for the call at bci, or -1 if there is no profile.
static int call_count(MethodData* mdo, int bci) {
  if (mdo == nullptr) {
    return -1;
  }
  MutexLocker ml(mdo->extra_data_lock(), Mutex::_no_safepoint_check_flag);
  NoSafepointVerifier nsv;
  ProfileData* data = mdo->bci_to_data(bci);
  if (data == nullptr || !data->is_CounterData()) {
    return -1;
  }
  int count = data->as_CounterData()->count();
  if (data->is_ReceiverTypeData()) {
    ReceiverTypeData* rtd = data->as_ReceiverTypeData();
    for (uint row = 0; row < ReceiverTypeData::row_limit(); row++) {
      count += rtd->receiver_count(row);
    }
  }
  return count;
}

// Lists call sites in the method and in everything C2 inlined into it, since
// an inline directive on the method applies to its whole compilation.
static void write_calls(JavaThread* thread, outputStream* out, HotMethod* hm) {
  const int max_calls = 48;
  int n = 0;
  for (int m = -1; m < hm->_inlined.length() && n < max_calls; m++) {
    methodHandle mh(thread, (m < 0) ? hm->_method : hm->_inlined.at(m));
    const char* caller = (m < 0) ? hm->_name : name_of(mh());
    Handle loader(thread, mh->method_holder()->class_loader());
    MethodData* mdo = mh->method_data();
    BytecodeStream bcs(mh);
    Bytecodes::Code code;
    while ((code = bcs.next()) >= 0 && n < max_calls) {
      if (code != Bytecodes::_invokevirtual && code != Bytecodes::_invokespecial &&
          code != Bytecodes::_invokestatic && code != Bytecodes::_invokeinterface) {
        continue;
      }
      int count = call_count(mdo, bcs.bci());
      if (count == 0) {
        continue;   // never executed, C2 compiles it as an uncommon trap
      }
      Bytecode_invoke inv(mh, bcs.bci());
      InstanceKlass* ik = SystemDictionary::find_instance_klass(thread, inv.klass(), loader);
      Method* callee = (ik == nullptr) ? nullptr :
          ik->uncached_lookup_method(inv.name(), inv.signature(), Klass::OverpassLookupMode::find);
      // MethodHandle/VarHandle invokes are signature polymorphic; C2 links them itself.
      bool intrinsic = (callee != nullptr) ? callee->intrinsic_id() != vmIntrinsics::_none :
          (ik != nullptr && MethodHandles::is_signature_polymorphic_name(ik, inv.name()));
      if (intrinsic) {
        continue;
      }

      const char* name;
      if (callee != nullptr) {
        name = name_of(callee);
      } else {
        stringStream ss;
        ss.print("%s::%s", inv.klass()->as_klass_external_name(), inv.name()->as_C_string());
        name = ss.as_string();
      }
      nmethod* callee_nm = (callee != nullptr) ? callee->code() : nullptr;
      int callee_c2_bytes = (callee_nm != nullptr && callee_nm->is_compiled_by_c2()) ? callee_nm->insts_size() : 0;

      out->print("%s\n       {\"caller\": \"%s\", \"callee\": \"%s\", "
                 "\"count\": %d, \"callee_bytecode_size\": %d, \"callee_has_loops\": %s, "
                 "\"callee_c2_code_bytes\": %d, \"inlined\": %s}",
                 n++ > 0 ? "," : "", caller, name,
                 count, (callee != nullptr) ? callee->code_size() : -1,
                 (callee != nullptr && callee->has_loops()) ? "true" : "false", callee_c2_bytes,
                 (callee != nullptr && hm->_inlined.contains(callee)) ? "true" : "false");
      hm->_callees.append(name);
    }
  }
}

void AIJitAgentThread::write_profile(JavaThread* thread, outputStream* out, uint round,
                                     int total_samples, GrowableArray<HotMethod*>* hot) {
  out->print_cr("{");
  out->print_cr("  \"round\": %u,", round);
  out->print_cr("  \"total_samples\": %d,", total_samples);
  out->print_cr("  \"c2_defaults\": {\"MaxInlineSize\": %zd, \"FreqInlineSize\": %zd, \"InlineSmallCode\": %zd, "
                "\"MaxInlineLevel\": %zd, \"LoopUnrollLimit\": %zd, \"LoopMaxUnroll\": %zd},",
                MaxInlineSize, FreqInlineSize, InlineSmallCode, MaxInlineLevel, LoopUnrollLimit, LoopMaxUnroll);
  out->print("  \"applied_hints\": [");
  for (int i = 0; i < _hints->length(); i++) {
    AIJitHint* h = _hints->at(i);
    out->print("%s\"%s %s %s\"", i > 0 ? ", " : "", h->_is_inline ? "inline" : "unroll", h->_method, h->_arg);
  }
  out->print_cr("],");

  out->print_cr("  \"methods\": [");
  for (int i = 0; i < hot->length(); i++) {
    HotMethod* hm = hot->at(i);
    methodHandle mh(thread, hm->_method);
    out->print_cr("    {\"name\": \"%s\", \"signature\": \"%s\", \"samples\": %d, \"bytecode_size\": %d,",
                  hm->_name, mh->signature()->as_C_string(), hm->_samples, mh->code_size());
    out->print_cr("     \"has_loops\": %s, \"osr\": %s, \"c2_code_bytes\": %d, \"invocations\": %d, \"backedges\": %d,",
                  mh->has_loops() ? "true" : "false", hm->_osr ? "true" : "false", hm->_code_bytes,
                  mh->invocation_count(), mh->backedge_count());
    out->print("     \"calls\": [");
    write_calls(thread, out, hm);
    out->print_cr("]}%s", (i + 1 < hot->length()) ? "," : "");
  }
  out->print_cr("  ]");
  out->print_cr("}");
}

void AIJitAgentThread::read_hints(const char* path, GrowableArray<HotMethod*>* hot,
                                  GrowableArray<HotMethod*>* changed) {
  FILE* f = os::fopen(path, "r");
  if (f == nullptr) {
    log_info(aijit)("No hints file %s", path);
    return;
  }
  char line[1024];
  while (fgets(line, sizeof(line), f) != nullptr) {
    char verb[16];
    AIJitHint hint;
    static_assert(AIJitHint::MaxNameLength == 256, "must match the sscanf widths");
    if (line[0] == '#' || sscanf(line, "%15s %255s %255s", verb, hint._method, hint._arg) != 3) {
      continue;
    }

    HotMethod* hm = nullptr;
    for (int i = 0; i < hot->length() && hm == nullptr; i++) {
      if (strcmp(hot->at(i)->_name, hint._method) == 0) {
        hm = hot->at(i);
      }
    }

    // Only accept hints about methods and calls we profiled, within bounds.
    const char* reject = nullptr;
    if (hm == nullptr) {
      reject = "method not in profile";
    } else if (strcmp(verb, "inline") == 0) {
      hint._is_inline = true;
      if (!hm->calls(hint._arg)) {
        reject = "callee not called from method";
      }
    } else if (strcmp(verb, "unroll") == 0) {
      hint._is_inline = false;
      char* end;
      long limit = strtol(hint._arg, &end, 10);
      if (*end != '\0' || limit < MinUnrollHint || limit > MaxUnrollHint) {
        reject = "unroll limit out of range";
      }
    } else {
      reject = "unknown verb";
    }
    if (reject != nullptr) {
      log_info(aijit)("Rejected hint (%s): %s %s %s", reject, verb, hint._method, hint._arg);
      continue;
    }
    log_info(aijit)("Accepted hint: %s %s %s", verb, hint._method, hint._arg);
    _hints->append(new AIJitHint(hint));
    changed->append_if_missing(hm);
  }
  fclose(f);
}

// Build one directive per changed method from all hints so far, e.g.
//   [{"match": "Foo::bar", "c2": {"inline": ["+Foo::baz"], "LoopUnrollLimit": 120}}]
void AIJitAgentThread::install_directives(GrowableArray<HotMethod*>* changed) {
  stringStream json;
  json.print("[");
  for (int i = 0; i < changed->length(); i++) {
    const char* name = changed->at(i)->_name;
    const char* unroll = nullptr;
    int inlines = 0;
    for (int j = 0; j < _hints->length(); j++) {
      AIJitHint* h = _hints->at(j);
      if (strcmp(h->_method, name) == 0) {
        if (h->_is_inline) {
          inlines++;
        } else {
          unroll = h->_arg;   // latest wins
        }
      }
    }

    json.print("%s{\"match\": \"%s\", \"c2\": {", i > 0 ? ", " : "", name);
    if (inlines > 0) {
      json.print("\"inline\": [");
      int k = 0;
      for (int j = 0; j < _hints->length(); j++) {
        AIJitHint* h = _hints->at(j);
        if (h->_is_inline && strcmp(h->_method, name) == 0) {
          json.print("%s\"+%s\"", k++ > 0 ? ", " : "", h->_arg);
        }
      }
      json.print("]%s", unroll != nullptr ? ", " : "");
    }
    if (unroll != nullptr) {
      json.print("\"LoopUnrollLimit\": %s", unroll);
    }
    json.print("}}");
  }
  json.print("]");

  stringStream st;
  int installed = DirectivesParser::parse_string(json.base(), &st, false);
  if (installed > 0) {
    log_info(aijit)("Installed %d directive(s): %s", installed, json.base());
  } else {
    log_warning(aijit)("Could not install directives %s: %s", json.base(), st.base());
  }
}

void AIJitAgentThread::recompile(HotMethod* hm, TRAPS) {
  methodHandle mh(THREAD, hm->_method);

  // Throw away the current code (as WB_DeoptimizeMethod does) so the new
  // directive takes effect, then ask for a tier 4 compile right away.
  DeoptimizationScope deopt_scope;
  {
    MutexLocker mu(THREAD, Compile_lock);
    mh->method_holder()->mark_osr_nmethods(&deopt_scope, mh());
    MutexLocker ml(NMethodState_lock, Mutex::_no_safepoint_check_flag);
    if (mh->code() != nullptr) {
      deopt_scope.mark(mh->code());
    }
  }
  deopt_scope.deoptimize_marked();

  CompileBroker::compile_method(mh, InvocationEntryBci, CompLevel_full_optimization,
                                mh->invocation_count(), nullptr, CompileTask::Reason_AIJit, CHECK);
  log_info(aijit)("Requested tier 4 recompilation of %s", hm->_name);
}

#endif // COMPILER2
