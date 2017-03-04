// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/inspector/v8-debugger-agent-impl.h"

#include <algorithm>

#include "src/inspector/injected-script.h"
#include "src/inspector/inspected-context.h"
#include "src/inspector/java-script-call-frame.h"
#include "src/inspector/protocol/Protocol.h"
#include "src/inspector/remote-object-id.h"
#include "src/inspector/script-breakpoint.h"
#include "src/inspector/search-util.h"
#include "src/inspector/string-util.h"
#include "src/inspector/v8-debugger-script.h"
#include "src/inspector/v8-debugger.h"
#include "src/inspector/v8-inspector-impl.h"
#include "src/inspector/v8-inspector-session-impl.h"
#include "src/inspector/v8-regex.h"
#include "src/inspector/v8-runtime-agent-impl.h"
#include "src/inspector/v8-stack-trace-impl.h"

#include "include/v8-inspector.h"

namespace v8_inspector {

using protocol::Array;
using protocol::Maybe;
using protocol::Debugger::BreakpointId;
using protocol::Debugger::CallFrame;
using protocol::Runtime::ExceptionDetails;
using protocol::Runtime::ScriptId;
using protocol::Runtime::StackTrace;
using protocol::Runtime::RemoteObject;

namespace DebuggerAgentState {
static const char javaScriptBreakpoints[] = "javaScriptBreakopints";
static const char pauseOnExceptionsState[] = "pauseOnExceptionsState";
static const char asyncCallStackDepth[] = "asyncCallStackDepth";
static const char blackboxPattern[] = "blackboxPattern";
static const char debuggerEnabled[] = "debuggerEnabled";

// Breakpoint properties.
static const char url[] = "url";
static const char isRegex[] = "isRegex";
static const char lineNumber[] = "lineNumber";
static const char columnNumber[] = "columnNumber";
static const char condition[] = "condition";
static const char skipAllPauses[] = "skipAllPauses";

}  // namespace DebuggerAgentState

V8DebuggerAgentImpl::V8DebuggerAgentImpl(
    V8InspectorSessionImpl* session, protocol::FrontendChannel* frontendChannel,
    protocol::DictionaryValue* state)
    : m_inspector(session->inspector()),
      m_debugger(m_inspector->debugger()),
      m_enabled(false),
      m_state(state),
      m_frontend(frontendChannel),
      m_isolate(session->inspector()->isolate()) {
}

V8DebuggerAgentImpl::~V8DebuggerAgentImpl() {}

bool V8DebuggerAgentImpl::enabled() { return m_enabled; }

void V8DebuggerAgentImpl::enable(ErrorString* errorString) {
  if (enabled()) return;

  m_enabled = true;
  m_debugger->enable();
}

void V8DebuggerAgentImpl::disable(ErrorString*) {
  if (!enabled()) return;

  m_enabled = false;
}

void V8DebuggerAgentImpl::restore() {
}

void V8DebuggerAgentImpl::setBreakpointsActive(ErrorString* errorString,
                                               bool active) {
}

void V8DebuggerAgentImpl::setSkipAllPauses(ErrorString*, bool skip) {
}

void V8DebuggerAgentImpl::setBreakpointByUrl(
    ErrorString* errorString, int lineNumber,
    const Maybe<String16>& optionalURL, const Maybe<String16>& optionalURLRegex,
    const Maybe<int>& optionalColumnNumber,
    const Maybe<String16>& optionalCondition, String16* outBreakpointId,
    std::unique_ptr<protocol::Array<protocol::Debugger::Location>>* locations) {
}

void V8DebuggerAgentImpl::setBreakpoint(
    ErrorString* errorString,
    std::unique_ptr<protocol::Debugger::Location> location,
    const Maybe<String16>& optionalCondition, String16* outBreakpointId,
    std::unique_ptr<protocol::Debugger::Location>* actualLocation) {
}

void V8DebuggerAgentImpl::removeBreakpoint(ErrorString* errorString,
                                           const String16& breakpointId) {
}

void V8DebuggerAgentImpl::continueToLocation(
    ErrorString* errorString,
    std::unique_ptr<protocol::Debugger::Location> location) {
}

void V8DebuggerAgentImpl::searchInContent(
    ErrorString* error, const String16& scriptId, const String16& query,
    const Maybe<bool>& optionalCaseSensitive,
    const Maybe<bool>& optionalIsRegex,
    std::unique_ptr<Array<protocol::Debugger::SearchMatch>>* results) {
}

void V8DebuggerAgentImpl::setScriptSource(
    ErrorString* errorString, const String16& scriptId,
    const String16& newContent, const Maybe<bool>& dryRun,
    Maybe<protocol::Array<protocol::Debugger::CallFrame>>* newCallFrames,
    Maybe<bool>* stackChanged, Maybe<StackTrace>* asyncStackTrace,
    Maybe<protocol::Runtime::ExceptionDetails>* optOutCompileError) {
}

void V8DebuggerAgentImpl::restartFrame(
    ErrorString* errorString, const String16& callFrameId,
    std::unique_ptr<Array<CallFrame>>* newCallFrames,
    Maybe<StackTrace>* asyncStackTrace) {
}

void V8DebuggerAgentImpl::getScriptSource(ErrorString* error,
                                          const String16& scriptId,
                                          String16* scriptSource) {
}

void V8DebuggerAgentImpl::schedulePauseOnNextStatement(
    const String16& breakReason,
    std::unique_ptr<protocol::DictionaryValue> data) {
}

void V8DebuggerAgentImpl::cancelPauseOnNextStatement() {
}

void V8DebuggerAgentImpl::pause(ErrorString* errorString) {
}

void V8DebuggerAgentImpl::resume(ErrorString* errorString) {
}

void V8DebuggerAgentImpl::stepOver(ErrorString* errorString) {
}

void V8DebuggerAgentImpl::stepInto(ErrorString* errorString) {
}

void V8DebuggerAgentImpl::stepOut(ErrorString* errorString) {
}

void V8DebuggerAgentImpl::setPauseOnExceptions(
    ErrorString* errorString, const String16& stringPauseState) {
}

void V8DebuggerAgentImpl::evaluateOnCallFrame(
    ErrorString* errorString, const String16& callFrameId,
    const String16& expression, const Maybe<String16>& objectGroup,
    const Maybe<bool>& includeCommandLineAPI, const Maybe<bool>& silent,
    const Maybe<bool>& returnByValue, const Maybe<bool>& generatePreview,
    std::unique_ptr<RemoteObject>* result,
    Maybe<protocol::Runtime::ExceptionDetails>* exceptionDetails) {
}

void V8DebuggerAgentImpl::setVariableValue(
    ErrorString* errorString, int scopeNumber, const String16& variableName,
    std::unique_ptr<protocol::Runtime::CallArgument> newValueArgument,
    const String16& callFrameId) {
}

void V8DebuggerAgentImpl::setAsyncCallStackDepth(ErrorString* errorString,
                                                 int depth) {
}

void V8DebuggerAgentImpl::setBlackboxPatterns(
    ErrorString* errorString,
    std::unique_ptr<protocol::Array<String16>> patterns) {
}

void V8DebuggerAgentImpl::setBlackboxedRanges(
    ErrorString* error, const String16& scriptId,
    std::unique_ptr<protocol::Array<protocol::Debugger::ScriptPosition>>
        inPositions) {
}

void V8DebuggerAgentImpl::willExecuteScript(int scriptId) {
}

void V8DebuggerAgentImpl::didExecuteScript() {
}

void V8DebuggerAgentImpl::didParseSource(
    std::unique_ptr<V8DebuggerScript> script, bool success) {
}

V8DebuggerAgentImpl::SkipPauseRequest V8DebuggerAgentImpl::didPause(
    v8::Local<v8::Context> context, v8::Local<v8::Value> exception,
    const std::vector<String16>& hitBreakpoints, bool isPromiseRejection) {
  return RequestNoSkip;
}

void V8DebuggerAgentImpl::didContinue() {
}

void V8DebuggerAgentImpl::breakProgram(
    const String16& breakReason,
    std::unique_ptr<protocol::DictionaryValue> data) {
}

void V8DebuggerAgentImpl::breakProgramOnException(
    const String16& breakReason,
    std::unique_ptr<protocol::DictionaryValue> data) {
}

void V8DebuggerAgentImpl::setBreakpointAt(const String16& scriptId,
                                          int lineNumber, int columnNumber,
                                          BreakpointSource source,
                                          const String16& condition) {
}

void V8DebuggerAgentImpl::removeBreakpointAt(const String16& scriptId,
                                             int lineNumber, int columnNumber,
                                             BreakpointSource source) {
}

void V8DebuggerAgentImpl::reset() {
}

}  // namespace v8_inspector
