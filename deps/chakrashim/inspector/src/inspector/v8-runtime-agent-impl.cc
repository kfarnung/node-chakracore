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
#include "src/jsrtinspectorhelpers.h"

namespace v8_inspector {

namespace V8RuntimeAgentImplState {
static const char customObjectFormatterEnabled[] =
    "customObjectFormatterEnabled";
static const char runtimeEnabled[] = "runtimeEnabled";
};

using protocol::Runtime::RemoteObject;

static bool hasInternalError(ErrorString* errorString, bool hasError) {
  if (hasError) *errorString = "Internal error";
  return hasError;
}

V8RuntimeAgentImpl::V8RuntimeAgentImpl(
    V8InspectorSessionImpl* session, protocol::FrontendChannel* FrontendChannel,
    protocol::DictionaryValue* state)
    : m_session(session),
      m_state(state),
      m_frontend(FrontendChannel),
      m_inspector(session->inspector()),
      m_enabled(false) {}

V8RuntimeAgentImpl::~V8RuntimeAgentImpl() {}

void V8RuntimeAgentImpl::evaluate(
    const String16& expression, const Maybe<String16>& objectGroup,
    const Maybe<bool>& includeCommandLineAPI, const Maybe<bool>& silent,
    const Maybe<int>& executionContextId, const Maybe<bool>& returnByValue,
    const Maybe<bool>& generatePreview, const Maybe<bool>& userGesture,
    const Maybe<bool>& awaitPromise,
    std::unique_ptr<EvaluateCallback> callback) {
  ErrorString errorString;

  JsValueRef expStr;
  if (JsCreateStringUtf16(expression.characters16(), expression.length(), &expStr) != JsNoError) {
    errorString = "Failed to create string";
    callback->sendFailure(errorString);
    return;
  }

  JsValueRef result;
  if (JsDiagEvaluate(expStr, 0, JsParseScriptAttributeNone, true, &result) != JsNoError) {
    errorString = "Failed to evaluate expression";
    callback->sendFailure(errorString);
    return;
  }

  Maybe<protocol::Runtime::ExceptionDetails> exceptionDetails;
  protocol::ErrorSupport errors;
  std::unique_ptr<protocol::Value> protocolValue =
      toProtocolValue(&errorString, v8::Context::GetCurrent(),
                      jsrt::InspectorHelpers::WrapEvaluateObject(result));
  if (!protocolValue) {
    callback.get()->sendSuccess(nullptr, exceptionDetails);
    return;
  }
  std::unique_ptr<protocol::Runtime::RemoteObject> remoteObject =
    protocol::Runtime::RemoteObject::parse(protocolValue.get(), &errors);
  if (!remoteObject) {
    errorString = errors.errors();
    callback.get()->sendFailure(errorString);
    return;
  }

  callback.get()->sendSuccess(std::move(remoteObject), exceptionDetails);
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

static std::unique_ptr<protocol::DictionaryValue> ParseObjectId(const String16& objectId) {
  std::unique_ptr<protocol::Value> parsedValue = protocol::parseJSON(objectId);
  if (!parsedValue || parsedValue->type() != protocol::Value::TypeObject)
    return nullptr;

  std::unique_ptr<protocol::DictionaryValue> parsedObjectId(
    protocol::DictionaryValue::cast(parsedValue.release()));

  return parsedObjectId;
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
  using protocol::Runtime::InternalPropertyDescriptor;

  *errorString = "Failure";

  if (!ownProperties.fromMaybe(false) || accessorPropertiesOnly.fromMaybe(false)) {
    // We don't support either of these lookups.
    return;
  }
  
  std::unique_ptr<protocol::DictionaryValue> parsedId = ParseObjectId(objectId);
  if (parsedId == nullptr) {
    return;
  }

  protocol::String id;
  int ordinal;
  if (parsedId->getString("id", &id)) {
    bool ok = false;
    int handle = id.toInteger(&ok);
    if (!ok) {
      *errorString = "Invalid object ID";
    }

    JsValueRef handleObject;
    JsDiagGetObjectFromHandle(handle, &handleObject);

    JsValueRef propertiesObject;
    JsDiagGetProperties(handle, 0, 1000, &propertiesObject);
  }
  else if (parsedId->getInteger("ordinal", &ordinal) && parsedId->getString("name", &id)) {
    JsValueRef stackProperties;
    JsDiagGetStackProperties(ordinal, &stackProperties);

    if (id == "locals") {
      JsValueRef locals;
      jsrt::InspectorHelpers::GetProperty(stackProperties, "locals", &locals);
    }
    else if (id == "globals") {
      JsValueRef globals;
      jsrt::InspectorHelpers::GetProperty(stackProperties, "globals", &globals);
    }
    else {
      *errorString = "Invalid object ID";
    }
  }
  else {
    *errorString = "Invalid object ID";
  }

  v8::Local<v8::Array> propertiesArray;

  *result = protocol::Array<protocol::Runtime::PropertyDescriptor>::create();

  std::unique_ptr<protocol::Array<InternalPropertyDescriptor>>
      propertiesProtocolArray =
          protocol::Array<InternalPropertyDescriptor>::create();
  /*for (uint32_t i = 0; i < propertiesArray->Length(); i += 2) {
    v8::Local<v8::Value> name;
    if (hasInternalError(
            errorString,
            !propertiesArray->Get(scope.context(), i).ToLocal(&name)) ||
        !name->IsString())
      return;
    v8::Local<v8::Value> value;
    if (hasInternalError(
            errorString,
            !propertiesArray->Get(scope.context(), i + 1).ToLocal(&value)))
      return;
    std::unique_ptr<RemoteObject> wrappedValue =
        scope.injectedScript()->wrapObject(errorString, value,
                                           scope.objectGroupName());
    if (!wrappedValue) return;
    propertiesProtocolArray->addItem(
        InternalPropertyDescriptor::create()
            .setName(toProtocolString(name.As<v8::String>()))
            .setValue(std::move(wrappedValue))
            .build());
  }*/
  if (!propertiesProtocolArray->length()) return;
  *internalProperties = std::move(propertiesProtocolArray);
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
  m_inspector->client()->beginEnsureAllContextsInGroup(
      m_session->contextGroupId());
  m_enabled = true;
  m_state->setBoolean(V8RuntimeAgentImplState::runtimeEnabled, true);
  m_inspector->enableStackCapturingIfNeeded();
  m_session->reportAllContexts(this);
  V8ConsoleMessageStorage* storage =
      m_inspector->ensureConsoleMessageStorage(m_session->contextGroupId());
  for (const auto& message : storage->messages()) {
    if (!reportMessage(message.get(), false)) return;
  }
}

void V8RuntimeAgentImpl::disable(ErrorString* errorString) {
  if (!m_enabled) return;

  m_enabled = false;
}

void V8RuntimeAgentImpl::reset() {
}

void V8RuntimeAgentImpl::reportExecutionContextCreated(
    InspectedContext* context) {
  if (!m_enabled) return;
  context->setReported(true);
  std::unique_ptr<protocol::Runtime::ExecutionContextDescription> description =
      protocol::Runtime::ExecutionContextDescription::create()
          .setId(context->contextId())
          .setName(context->humanReadableName())
          .setOrigin(context->origin())
          .build();
  if (!context->auxData().isEmpty())
    description->setAuxData(protocol::DictionaryValue::cast(
        protocol::parseJSON(context->auxData())));
  m_frontend.executionContextCreated(std::move(description));
}

void V8RuntimeAgentImpl::reportExecutionContextDestroyed(
    InspectedContext* context) {
}

void V8RuntimeAgentImpl::inspect(
    std::unique_ptr<protocol::Runtime::RemoteObject> objectToInspect,
    std::unique_ptr<protocol::DictionaryValue> hints) {
}

void V8RuntimeAgentImpl::messageAdded(V8ConsoleMessage* message) {
  if (m_enabled) reportMessage(message, true);
}

bool V8RuntimeAgentImpl::reportMessage(V8ConsoleMessage* message,
  bool generatePreview) {
  message->reportToFrontend(&m_frontend, m_session, generatePreview);
  m_frontend.flush();
  return m_inspector->hasConsoleMessageStorage(m_session->contextGroupId());
}

}  // namespace v8_inspector
