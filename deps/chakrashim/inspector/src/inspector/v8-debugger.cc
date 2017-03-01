// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/inspector/v8-debugger.h"

#include "src/inspector/protocol/Protocol.h"
#include "src/inspector/string-util.h"
#include "src/inspector/v8-debugger-agent-impl.h"
#include "src/inspector/v8-inspector-impl.h"
#include "src/inspector/v8-stack-trace-impl.h"

namespace v8_inspector {

V8Debugger::V8Debugger(v8::Isolate* isolate, V8InspectorImpl* inspector)
    : m_inspector(inspector),
      m_enableCount(0),
      m_breakpointsActivated(true),
      m_maxAsyncCallStackDepth(0) {}

V8Debugger::~V8Debugger() {}

void V8Debugger::enable() {
  if (m_enableCount++) return;
}

void V8Debugger::disable() {
  if (--m_enableCount) return;
}

bool V8Debugger::enabled() const { return m_enableCount > 0; }

// static
int V8Debugger::contextId(v8::Local<v8::Context> context) {
    return 0;
}

// static
int V8Debugger::getGroupId(v8::Local<v8::Context> context) {
    return 0;
}

void V8Debugger::getCompiledScripts(
    int contextGroupId,
    std::vector<std::unique_ptr<V8DebuggerScript>>& result) {
}

String16 V8Debugger::setBreakpoint(const String16& sourceID,
                                   const ScriptBreakpoint& scriptBreakpoint,
                                   int* actualLineNumber,
                                   int* actualColumnNumber) {
    return String16();
}

void V8Debugger::removeBreakpoint(const String16& breakpointId) {
}

void V8Debugger::setBreakpointsActivated(bool activated) {
}

V8Debugger::PauseOnExceptionsState V8Debugger::getPauseOnExceptionsState() {
    return V8Debugger::PauseOnExceptionsState::DontPauseOnExceptions;
}

void V8Debugger::setPauseOnExceptionsState(
    PauseOnExceptionsState pauseOnExceptionsState) {
}

void V8Debugger::setPauseOnNextStatement(bool pause) {
}

bool V8Debugger::canBreakProgram() {
    return false;
}

void V8Debugger::breakProgram() {
}

void V8Debugger::continueProgram() {
}

void V8Debugger::stepIntoStatement() {
}

void V8Debugger::stepOverStatement() {
}

void V8Debugger::stepOutOfFunction() {
}

void V8Debugger::clearStepping() {
}

bool V8Debugger::setScriptSource(
    const String16& sourceID, v8::Local<v8::String> newSource, bool dryRun,
    ErrorString* error,
    Maybe<protocol::Runtime::ExceptionDetails>* exceptionDetails,
    JavaScriptCallFrames* newCallFrames, Maybe<bool>* stackChanged) {
    return false;
}

JavaScriptCallFrames V8Debugger::currentCallFrames(int limit) {
    return JavaScriptCallFrames();
}

V8StackTraceImpl* V8Debugger::currentAsyncCallChain() {
    return nullptr;
}

v8::MaybeLocal<v8::Array> V8Debugger::internalProperties(
    v8::Local<v8::Context> context, v8::Local<v8::Value> value) {
    return v8::MaybeLocal<v8::Array>();
}

bool V8Debugger::isPaused() { return !m_pausedContext.IsEmpty(); }

std::unique_ptr<V8StackTraceImpl> V8Debugger::createStackTrace(
    v8::Local<v8::StackTrace> stackTrace) {
    return std::unique_ptr<V8StackTraceImpl>(nullptr);
}

int V8Debugger::markContext(const V8ContextInfo& info) {
    return 0;
}

void V8Debugger::setAsyncCallStackDepth(V8DebuggerAgentImpl* agent, int depth) {
}

void V8Debugger::asyncTaskScheduled(const StringView& taskName, void* task,
                                    bool recurring) {
}

void V8Debugger::asyncTaskScheduled(const String16& taskName, void* task,
                                    bool recurring) {
}

void V8Debugger::asyncTaskCanceled(void* task) {
}

void V8Debugger::asyncTaskStarted(void* task) {
}

void V8Debugger::asyncTaskFinished(void* task) {
}

void V8Debugger::allAsyncTasksCanceled() {
}

void V8Debugger::muteScriptParsedEvents() {
}

void V8Debugger::unmuteScriptParsedEvents() {
}

std::unique_ptr<V8StackTraceImpl> V8Debugger::captureStackTrace(
    bool fullStack) {
    return std::unique_ptr<V8StackTraceImpl>(nullptr);
}

}  // namespace v8_inspector
