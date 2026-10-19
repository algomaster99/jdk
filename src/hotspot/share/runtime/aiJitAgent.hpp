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
#ifndef SHARE_RUNTIME_AIJITAGENT_HPP
#define SHARE_RUNTIME_AIJITAGENT_HPP

#include "runtime/javaThread.hpp"
#include "utilities/growableArray.hpp"

class AIJitHint;
class Candidates;
class HotMethod;
class outputStream;

// A hidden JavaThread that closes a profile -> agent -> recompile loop for C2.
//
// Each round it samples which C2 nmethods are executing, writes a JSON
// profile of the hottest ones (call sites, callee sizes, what C2 already
// inlined) and runs '<AIJitAgentCommand> <profile.json> <hints.txt>'.
// The agent, typically an LLM, answers with one hint per line:
//
//   inline <method> <callee>   force inlining of <callee> into <method>
//   unroll <method> <limit>    per-method LoopUnrollLimit for <method>
//
// Hints are checked against the profile, installed as compiler directives,
// and the method is deoptimized and recompiled at tier 4. A hint only steers
// existing C2 heuristics, so a bad hint can make code slower but never wrong.
class AIJitAgentThread : public JavaThread {
 private:
  // All hints accepted so far. A new directive for a method shadows older
  // ones on the DirectivesStack, so it is rebuilt from the full history.
  static GrowableArrayCHeap<AIJitHint*, mtCompiler>* _hints;

  AIJitAgentThread() : JavaThread(thread_entry) {}

  static void thread_entry(JavaThread* thread, TRAPS);
  static void run_round(JavaThread* thread, uint round, TRAPS);

  static void collect_hot_methods(JavaThread* thread, Candidates& candidates, GrowableArray<HotMethod*>* hot);
  static void write_profile(JavaThread* thread, outputStream* out, uint round, int total_samples,
                            GrowableArray<HotMethod*>* hot);
  static void read_hints(const char* path, GrowableArray<HotMethod*>* hot,
                         GrowableArray<HotMethod*>* changed);
  static void install_directives(GrowableArray<HotMethod*>* changed);
  static void recompile(HotMethod* hm, TRAPS);

 public:
  static void initialize();

  // so the agent never samples its own functions
  bool is_hidden_from_external_view() const { return true; }
};

#endif // SHARE_RUNTIME_AIJITAGENT_HPP
#endif // COMPILER2
