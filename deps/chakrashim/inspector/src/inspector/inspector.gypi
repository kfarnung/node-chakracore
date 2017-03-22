# Copyright 2016 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'inspector_generated_sources': [
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Forward.h',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Protocol.cpp',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Protocol.h',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Console.cpp',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Console.h',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Debugger.cpp',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Debugger.h',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/HeapProfiler.cpp',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/HeapProfiler.h',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Profiler.cpp',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Profiler.h',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Runtime.cpp',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Runtime.h',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Schema.cpp',
      '<(SHARED_INTERMEDIATE_DIR)/src/inspector/protocol/Schema.h',
      '<(SHARED_INTERMEDIATE_DIR)/include/inspector/Debugger.h',
      '<(SHARED_INTERMEDIATE_DIR)/include/inspector/Runtime.h',
      '<(SHARED_INTERMEDIATE_DIR)/include/inspector/Schema.h',
    ],

    'inspector_all_sources': [
      '<@(inspector_generated_sources)',
      '../../include/v8-inspector.h',
      '../../include/v8-inspector-protocol.h',
      'inspected-context.cc',
      'inspected-context.h',
      'java-script-call-frame.cc',
      'java-script-call-frame.h',
      'protocol-platform.h',
      'remote-object-id.cc',
      'remote-object-id.h',
      'script-breakpoint.h',
      'search-util.cc',
      'search-util.h',
      'string-16.cc',
      'string-16.h',
      'string-util.cc',
      'string-util.h',
      'v8-console.cc',
      'v8-console.h',
      'v8-console-agent-impl.cc',
      'v8-console-agent-impl.h',
      'v8-console-message.cc',
      'v8-console-message.h',
      'v8-debugger.cc',
      'v8-debugger.h',
      'v8-debugger-agent-impl.cc',
      'v8-debugger-agent-impl.h',
      'v8-debugger-script.cc',
      'v8-debugger-script.h',
      'v8-function-call.cc',
      'v8-function-call.h',
      'v8-heap-profiler-agent-impl.cc',
      'v8-heap-profiler-agent-impl.h',
      'v8-inspector-impl.cc',
      'v8-inspector-impl.h',
      'v8-inspector-session-impl.cc',
      'v8-inspector-session-impl.h',
      'v8-internal-value-type.cc',
      'v8-internal-value-type.h',
      'v8-profiler-agent-impl.cc',
      'v8-profiler-agent-impl.h',
      'v8-regex.cc',
      'v8-regex.h',
      'v8-runtime-agent-impl.cc',
      'v8-runtime-agent-impl.h',
      'v8-schema-agent-impl.cc',
      'v8-schema-agent-impl.h',
      'v8-stack-trace-impl.cc',
      'v8-stack-trace-impl.h',
      'v8-value-copier.cc',
      'v8-value-copier.h',
    ]
  }
}
