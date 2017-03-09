// Copyright Microsoft. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#pragma once

#include <v8.h>

namespace jsrt {

class InspectorHelpers {
public:
  static JsErrorCode HasProperty(JsValueRef obj, const char *name,
                                 bool *hasProperty);
  static JsErrorCode GetProperty(JsValueRef obj, const char *name,
                                 JsValueRef *value);
  static JsErrorCode GetBoolProperty(JsValueRef obj, const char *name,
                                     bool *value);
  static JsErrorCode GetIntProperty(JsValueRef obj, const char *name,
                                    int *value);
  static JsErrorCode SetProperty(JsValueRef obj, const char *name,
                                 JsValueRef value);
  static JsErrorCode SetPropertyString(JsValueRef obj, const char *name,
                                       const char *value);
  static JsErrorCode SetIndexedProperty(JsValueRef obj, int index,
                                        JsValueRef value);
  static JsErrorCode CopyPropertyIfPresent(JsValueRef sourceObj, 
                                           const char *sourceName,
                                           JsValueRef destObj,
                                           const char *destName = nullptr,
                                           bool *wasCopied = nullptr);
  static JsErrorCode CopyPropertyStringIfPresent(
      JsValueRef sourceObj,
      const char *sourceName,
      JsValueRef destObj,
      const char *destName = nullptr,
      bool *wasCopied = nullptr);

  static v8::Local<v8::Value> WrapEvaluateObject(JsValueRef sourceObject);
  static v8::Local<v8::Object> WrapCallFrameDetails(JsValueRef callFrame);
  static v8::Local<v8::Value> WrapRuntimeException(JsValueRef eventData);
  static JsRuntimeHandle GetRuntimeFromIsolate(v8::Isolate *isolate);
};

}  // namespace jsrt
