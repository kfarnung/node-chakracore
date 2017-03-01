// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_INSPECTOR_V8PROFILERAGENTIMPL_H_
#define V8_INSPECTOR_V8PROFILERAGENTIMPL_H_

#include <vector>

#include "src/base/macros.h"
#include "src/inspector/protocol/Forward.h"
#include "src/inspector/protocol/Profiler.h"

namespace v8_inspector {

class V8InspectorSessionImpl;

using protocol::ErrorString;

class V8ProfilerAgentImpl : public protocol::Profiler::Backend {
 public:
  V8ProfilerAgentImpl(V8InspectorSessionImpl*, protocol::FrontendChannel*,
                      protocol::DictionaryValue* state);
  ~V8ProfilerAgentImpl() override;

  bool enabled() const { return m_enabled; }
  void restore();

  void enable(ErrorString*) override;
  void disable(ErrorString*) override;
  void setSamplingInterval(ErrorString*, int) override;
  void start(ErrorString*) override;
  void stop(ErrorString*,
            std::unique_ptr<protocol::Profiler::Profile>*) override;

  void consoleProfile(const String16& title);
  void consoleProfileEnd(const String16& title);

  bool idleStarted();
  bool idleFinished();

  void collectSample();

 private:
  bool m_enabled;
  
  DISALLOW_COPY_AND_ASSIGN(V8ProfilerAgentImpl);
};

}  // namespace v8_inspector

#endif  // V8_INSPECTOR_V8PROFILERAGENTIMPL_H_
