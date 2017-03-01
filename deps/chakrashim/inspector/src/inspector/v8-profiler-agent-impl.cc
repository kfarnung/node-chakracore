// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/inspector/v8-profiler-agent-impl.h"

#include <vector>

#include "src/inspector/protocol/Protocol.h"
#include "src/inspector/string-util.h"
#include "src/inspector/v8-inspector-impl.h"
#include "src/inspector/v8-inspector-session-impl.h"
#include "src/inspector/v8-stack-trace-impl.h"

namespace v8_inspector {

V8ProfilerAgentImpl::V8ProfilerAgentImpl(
    V8InspectorSessionImpl* session, protocol::FrontendChannel* frontendChannel,
    protocol::DictionaryValue* state)
    : m_enabled(false) {}

V8ProfilerAgentImpl::~V8ProfilerAgentImpl() {
}

void V8ProfilerAgentImpl::consoleProfile(const String16& title) {
}

void V8ProfilerAgentImpl::consoleProfileEnd(const String16& title) {
}

void V8ProfilerAgentImpl::enable(ErrorString*) {
}

void V8ProfilerAgentImpl::disable(ErrorString* errorString) {
}

void V8ProfilerAgentImpl::setSamplingInterval(ErrorString* error,
                                              int interval) {
}

void V8ProfilerAgentImpl::restore() {
}

void V8ProfilerAgentImpl::start(ErrorString* error) {
}

void V8ProfilerAgentImpl::stop(
    ErrorString* errorString,
    std::unique_ptr<protocol::Profiler::Profile>* profile) {
}

bool V8ProfilerAgentImpl::idleStarted() {
    return false;
}

bool V8ProfilerAgentImpl::idleFinished() {
    return false;
}

void V8ProfilerAgentImpl::collectSample() {
}

}  // namespace v8_inspector
