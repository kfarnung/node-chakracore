// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/inspector/v8-heap-profiler-agent-impl.h"

#include "src/inspector/protocol/Protocol.h"
#include "src/inspector/string-util.h"
#include "src/inspector/v8-debugger.h"
#include "src/inspector/v8-inspector-impl.h"
#include "src/inspector/v8-inspector-session-impl.h"

#include "include/v8-inspector.h"
#include "include/v8-profiler.h"
#include "include/v8-version.h"

namespace v8_inspector {

V8HeapProfilerAgentImpl::V8HeapProfilerAgentImpl(
    V8InspectorSessionImpl* session, protocol::FrontendChannel* frontendChannel,
    protocol::DictionaryValue* state) {}

V8HeapProfilerAgentImpl::~V8HeapProfilerAgentImpl() {}

void V8HeapProfilerAgentImpl::restore() {
}

void V8HeapProfilerAgentImpl::collectGarbage(ErrorString*) {
}

void V8HeapProfilerAgentImpl::startTrackingHeapObjects(
    ErrorString*, const protocol::Maybe<bool>& trackAllocations) {
}

void V8HeapProfilerAgentImpl::stopTrackingHeapObjects(
    ErrorString* error, const protocol::Maybe<bool>& reportProgress) {
}

void V8HeapProfilerAgentImpl::enable(ErrorString*) {
}

void V8HeapProfilerAgentImpl::disable(ErrorString* error) {
}

void V8HeapProfilerAgentImpl::takeHeapSnapshot(
    ErrorString* errorString, const protocol::Maybe<bool>& reportProgress) {
}

void V8HeapProfilerAgentImpl::getObjectByHeapObjectId(
    ErrorString* error, const String16& heapSnapshotObjectId,
    const protocol::Maybe<String16>& objectGroup,
    std::unique_ptr<protocol::Runtime::RemoteObject>* result) {
}

void V8HeapProfilerAgentImpl::addInspectedHeapObject(
    ErrorString* errorString, const String16& inspectedHeapObjectId) {
}

void V8HeapProfilerAgentImpl::getHeapObjectId(ErrorString* errorString,
                                              const String16& objectId,
                                              String16* heapSnapshotObjectId) {
}

void V8HeapProfilerAgentImpl::startSampling(
    ErrorString* errorString, const Maybe<double>& samplingInterval) {
}


void V8HeapProfilerAgentImpl::stopSampling(
    ErrorString* errorString,
    std::unique_ptr<protocol::HeapProfiler::SamplingHeapProfile>* profile) {
}

}  // namespace v8_inspector
