/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "src/inspector/v8-runtime-agent-impl.h"

#include "src/inspector/injected-script.h"
#include "src/inspector/inspected-context.h"
#include "src/inspector/protocol/Protocol.h"
#include "src/inspector/remote-object-id.h"
#include "src/inspector/string-util.h"
#include "src/inspector/v8-console-message.h"
#include "src/inspector/v8-debugger-agent-impl.h"
#include "src/inspector/v8-debugger.h"
#include "src/inspector/v8-inspector-impl.h"
#include "src/inspector/v8-inspector-session-impl.h"
#include "src/inspector/v8-stack-trace-impl.h"

#include "include/v8-inspector.h"

namespace v8_inspector {

V8RuntimeAgentImpl::V8RuntimeAgentImpl(
    V8InspectorSessionImpl* session, protocol::FrontendChannel* FrontendChannel,
    protocol::DictionaryValue* state)
    : m_enabled(false) {}

V8RuntimeAgentImpl::~V8RuntimeAgentImpl() {}

void V8RuntimeAgentImpl::evaluate(
    const String16& expression, const Maybe<String16>& objectGroup,
    const Maybe<bool>& includeCommandLineAPI, const Maybe<bool>& silent,
    const Maybe<int>& executionContextId, const Maybe<bool>& returnByValue,
    const Maybe<bool>& generatePreview, const Maybe<bool>& userGesture,
    const Maybe<bool>& awaitPromise,
    std::unique_ptr<EvaluateCallback> callback) {
}

void V8RuntimeAgentImpl::awaitPromise(
    const String16& promiseObjectId, const Maybe<bool>& returnByValue,
    const Maybe<bool>& generatePreview,
    std::unique_ptr<AwaitPromiseCallback> callback) {
}

void V8RuntimeAgentImpl::callFunctionOn(
    const String16& objectId, const String16& expression,
    const Maybe<protocol::Array<protocol::Runtime::CallArgument>>&
        optionalArguments,
    const Maybe<bool>& silent, const Maybe<bool>& returnByValue,
    const Maybe<bool>& generatePreview, const Maybe<bool>& userGesture,
    const Maybe<bool>& awaitPromise,
    std::unique_ptr<CallFunctionOnCallback> callback) {
}

void V8RuntimeAgentImpl::getProperties(
    ErrorString* errorString, const String16& objectId,
    const Maybe<bool>& ownProperties, const Maybe<bool>& accessorPropertiesOnly,
    const Maybe<bool>& generatePreview,
    std::unique_ptr<protocol::Array<protocol::Runtime::PropertyDescriptor>>*
        result,
    Maybe<protocol::Array<protocol::Runtime::InternalPropertyDescriptor>>*
        internalProperties,
    Maybe<protocol::Runtime::ExceptionDetails>* exceptionDetails) {
}

void V8RuntimeAgentImpl::releaseObject(ErrorString* errorString,
                                       const String16& objectId) {
}

void V8RuntimeAgentImpl::releaseObjectGroup(ErrorString*,
                                            const String16& objectGroup) {
}

void V8RuntimeAgentImpl::runIfWaitingForDebugger(ErrorString* errorString) {
}

void V8RuntimeAgentImpl::setCustomObjectFormatterEnabled(ErrorString*,
                                                         bool enabled) {
}

void V8RuntimeAgentImpl::discardConsoleEntries(ErrorString*) {
}

void V8RuntimeAgentImpl::compileScript(
    ErrorString* errorString, const String16& expression,
    const String16& sourceURL, bool persistScript,
    const Maybe<int>& executionContextId, Maybe<String16>* scriptId,
    Maybe<protocol::Runtime::ExceptionDetails>* exceptionDetails) {
}

void V8RuntimeAgentImpl::runScript(
    const String16& scriptId, const Maybe<int>& executionContextId,
    const Maybe<String16>& objectGroup, const Maybe<bool>& silent,
    const Maybe<bool>& includeCommandLineAPI, const Maybe<bool>& returnByValue,
    const Maybe<bool>& generatePreview, const Maybe<bool>& awaitPromise,
    std::unique_ptr<RunScriptCallback> callback) {
}

void V8RuntimeAgentImpl::restore() {
}

void V8RuntimeAgentImpl::enable(ErrorString* errorString) {
  if (m_enabled) return;

  m_enabled = true;
}

void V8RuntimeAgentImpl::disable(ErrorString* errorString) {
  if (!m_enabled) return;

  m_enabled = false;
}

void V8RuntimeAgentImpl::reset() {
}

void V8RuntimeAgentImpl::reportExecutionContextCreated(
    InspectedContext* context) {
}

void V8RuntimeAgentImpl::reportExecutionContextDestroyed(
    InspectedContext* context) {
}

void V8RuntimeAgentImpl::inspect(
    std::unique_ptr<protocol::Runtime::RemoteObject> objectToInspect,
    std::unique_ptr<protocol::DictionaryValue> hints) {
}

void V8RuntimeAgentImpl::messageAdded(V8ConsoleMessage* message) {
}

}  // namespace v8_inspector
