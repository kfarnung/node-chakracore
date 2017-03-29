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

#include <string>

#include "v8chakra.h"
#include "jsrtinspectorhelpers.h"

namespace jsrt {
  static int JsrtDebugPropertyAttributeReadOnly = 4;

  typedef JsErrorCode(*ConvertFunc)(JsValueRef, JsValueRef*);

  template<class ValueConvertFunc>
  static JsErrorCode CopyPropertyImpl(
      JsValueRef sourceObj,
      const char *sourceName,
      JsValueRef destObj,
      const char *destName,
      ValueConvertFunc convertFunc,
      bool *wasCopied) {
    JsErrorCode err = JsNoError;

    if (wasCopied != nullptr)
    {
      *wasCopied = false;
    }

    JsPropertyIdRef propId;
    err = JsCreatePropertyId(sourceName, strlen(sourceName), &propId);
    if (err != JsNoError) {
      return err;
    }

    bool hasProp;
    err = JsHasProperty(sourceObj, propId, &hasProp);
    if (err != JsNoError) {
      return err;
    }

    if (!hasProp) {
      return err;
    }

    JsValueRef sourceVal = nullptr;
    err = JsGetProperty(sourceObj, propId, &sourceVal);
    if (err != JsNoError) {
      return err;
    }

    JsValueRef destVal = nullptr;
    if (convertFunc != nullptr) {
      err = convertFunc(sourceVal, &destVal);
      if (err != JsNoError) {
        return err;
      }
    }
    else {
      destVal = sourceVal;
    }

    if (destName != nullptr) {
      err = JsCreatePropertyId(destName, strlen(destName), &propId);
      if (err != JsNoError) {
        return err;
      }
    }

    err = JsSetProperty(destObj, propId, destVal, true);
    if (err != JsNoError) {
      return err;
    }

    if (wasCopied != nullptr)
    {
      *wasCopied = true;
    }

    return err;
  }

  static bool CopyPropertyIfPresent(JsValueRef sourceObj,
                                    const char *sourceName,JsValueRef destObj,
                                    const char *destName = nullptr) {
    bool wasCopied;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::TryCopyProperty(
        sourceObj, sourceName, destObj, destName, &wasCopied));

    return wasCopied;
  }

  static bool CopyPropertyStringIfPresent(JsValueRef sourceObj,
                                          const char *sourceName,
                                          JsValueRef destObj,
                                          const char *destName = nullptr) {
    bool wasCopied;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::TryCopyPropertyString(
        sourceObj, sourceName, destObj, destName, &wasCopied));

    return wasCopied;
  }

  static std::string GetObjectIdForFrameProp(unsigned int frameIndex,
                                             const char *propName) {
    std::string objectId("{\"ordinal\":");
    objectId.append(std::to_string(frameIndex));
    objectId.append(",\"name\":\"");
    objectId.append(propName);
    objectId.append("\"}");

    return objectId;
  }

  static std::string GetObjectIdForHandle(unsigned int handle) {
    std::string objectId("{\"handle\":");
    objectId.append(std::to_string(handle));
    objectId.append("}");

    return objectId;
  }

  JsErrorCode InspectorHelpers::HasProperty(JsValueRef obj, const char *name,
                                            bool *hasProperty) {
    JsErrorCode err = JsNoError;

    JsPropertyIdRef propId;
    err = JsCreatePropertyId(name, strlen(name), &propId);
    if (err != JsNoError) {
      return err;
    }

    err = JsHasProperty(obj, propId, hasProperty);
    if (err != JsNoError) {
      return err;
    }

    return err;
  }

  JsErrorCode InspectorHelpers::GetProperty(JsValueRef obj, const char *name,
                                            JsValueRef *value) {
    JsErrorCode err = JsNoError;

    JsPropertyIdRef propId;
    err = JsCreatePropertyId(name, strlen(name), &propId);
    if (err != JsNoError) {
      return err;
    }

    err = JsGetProperty(obj, propId, value);
    if (err != JsNoError) {
      return err;
    }

    return err;
  }

  JsErrorCode InspectorHelpers::GetBoolProperty(JsValueRef obj,
                                                const char *name,
                                                bool *value) {
    JsErrorCode err = JsNoError;

    JsValueRef propVal = nullptr;
    err = GetProperty(obj, name, &propVal);
    if (err != JsNoError) {
      return err;
    }

    err = ValueToNative</*LIKELY*/true>(JsConvertValueToBoolean,
                                        JsBooleanToBool, propVal, value);
    if (err != JsNoError) {
      return err;
    }

    return err;
  }

  JsErrorCode InspectorHelpers::GetIntProperty(JsValueRef obj,
                                               const char *name,
                                               int *value) {
    JsErrorCode err = JsNoError;

    JsValueRef propVal = nullptr;
    err = GetProperty(obj, name, &propVal);
    if (err != JsNoError) {
      return err;
    }

    err = ValueToIntLikely(propVal, value);
    if (err != JsNoError) {
      return err;
    }

    return err;
  }

  JsErrorCode InspectorHelpers::GetIndexedProperty(JsValueRef obj, int index,
                                                   JsValueRef *value) {
    JsErrorCode err = JsNoError;

    JsValueRef indexVal = nullptr;
    err = JsIntToNumber(index, &indexVal);
    if (err != JsNoError) {
      return err;
    }

    err = JsGetIndexedProperty(obj, indexVal, value);
    if (err != JsNoError) {
      return err;
    }

    return err;
  }

  JsErrorCode InspectorHelpers::SetProperty(JsValueRef obj, const char *name,
                                            JsValueRef value) {
    JsErrorCode err = JsNoError;

    JsPropertyIdRef propId = nullptr;
    err = JsCreatePropertyId(name, strlen(name), &propId);
    if (err != JsNoError) {
      return err;
    }

    err = JsSetProperty(obj, propId, value, true);
    if (err != JsNoError) {
      return err;
    }

    return err;
  }

  JsErrorCode InspectorHelpers::SetBoolProperty(JsValueRef obj,
                                                const char *name,
                                                bool value) {
    JsErrorCode err = JsNoError;

    JsValueRef boolVal = nullptr;
    err = JsBoolToBoolean(value, &boolVal);
    if (err != JsNoError) {
      return err;
    }

    JsPropertyIdRef propId = nullptr;
    err = JsCreatePropertyId(name, strlen(name), &propId);
    if (err != JsNoError) {
      return err;
    }

    err = JsSetProperty(obj, propId, boolVal, true);
    if (err != JsNoError) {
      return err;
    }

    return err;
  }

  JsErrorCode InspectorHelpers::SetStringProperty(JsValueRef obj,
                                                  const char *name,
                                                  const char *value) {
    JsErrorCode err = JsNoError;

    JsValueRef stringVal = nullptr;
    err = JsCreateString(value, strlen(value), &stringVal);
    if (err != JsNoError) {
      return err;
    }

    JsPropertyIdRef propId = nullptr;
    err = JsCreatePropertyId(name, strlen(name), &propId);
    if (err != JsNoError) {
      return err;
    }

    err = JsSetProperty(obj, propId, stringVal, true);
    if (err != JsNoError) {
      return err;
    }

    return err;
  }

  JsErrorCode InspectorHelpers::SetIndexedProperty(JsValueRef obj, int index,
                                                   JsValueRef value) {
    JsErrorCode err = JsNoError;

    JsValueRef indexVal = nullptr;
    err = JsIntToNumber(index, &indexVal);
    if (err != JsNoError) {
      return err;
    }

    err = JsSetIndexedProperty(obj, indexVal, value);
    if (err != JsNoError) {
      return err;
    }

    return err;
  }

  JsErrorCode InspectorHelpers::TryCopyProperty(
      JsValueRef sourceObj,
      const char *sourceName,
      JsValueRef destObj,
      const char *destName,
      bool *wasCopied) {
    return CopyPropertyImpl(sourceObj, sourceName, destObj, destName,
                            static_cast<ConvertFunc>(nullptr), wasCopied);
  }

  JsErrorCode InspectorHelpers::TryCopyPropertyString(
      JsValueRef sourceObj,
      const char *sourceName,
      JsValueRef destObj,
      const char *destName,
      bool *wasCopied) {
    return CopyPropertyImpl(sourceObj, sourceName, destObj, destName,
                            &JsConvertValueToString, wasCopied);
  }

  JsErrorCode InspectorHelpers::ArrayConcat(JsValueRef array, JsValueRef value) {
    JsErrorCode err = JsNoError;

    if (array == nullptr) {
      return JsErrorInvalidArgument;
    }

    int arrayLength;
    err = InspectorHelpers::GetIntProperty(array, "length", &arrayLength);
    if (err != JsNoError) {
      return JsErrorInvalidArgument;
    }

    bool valueHasLength;
    err = InspectorHelpers::HasProperty(value, "length", &valueHasLength);
    if (err != JsNoError) {
      return err;
    }

    if (valueHasLength) {
      int valueLength;
      err = InspectorHelpers::GetIntProperty(value, "length", &valueLength);
      if (err != JsNoError) {
        return JsErrorInvalidArgument;
      }

      for (int i = 0; i < valueLength; i++) {
        JsValueRef valueObj = nullptr;
        err = InspectorHelpers::GetIndexedProperty(value, i, &valueObj);
        if (err != JsNoError) {
          return err;
        }

        err = InspectorHelpers::SetIndexedProperty(array, arrayLength + i, valueObj);
        if (err != JsNoError) {
          return err;
        }
      }
    }
    else {
      err = InspectorHelpers::SetIndexedProperty(array, arrayLength, value);
      if (err != JsNoError) {
        return err;
      }
    }

    return err;
  }

  v8::Local<v8::Value> InspectorHelpers::WrapEvaluateObject(
      JsValueRef sourceObject) {
    JsValueRef resultObj = nullptr;
    WrapObject(sourceObject, &resultObj);

    return v8::Utils::ToLocal(static_cast<v8::Value*>(resultObj));
  }

  v8::Local<v8::Object> InspectorHelpers::WrapCallFrameDetails(
      JsValueRef callFrame) {
    JsValueRef wrappedObj = nullptr;
    CHAKRA_VERIFY_NOERROR(JsCreateObject(&wrappedObj));

    {
      int functionHandle = 0;
      CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(callFrame,
                                                             "functionHandle",
                                                             &functionHandle));

      JsValueRef funcObj = nullptr;
      CHAKRA_VERIFY_NOERROR(JsDiagGetObjectFromHandle(functionHandle,
                                                      &funcObj));

      CHAKRA_VERIFY(CopyPropertyIfPresent(funcObj, "name", wrappedObj,
                                          "functionName"));

      {
        JsValueRef functionLocation = nullptr;
        CHAKRA_VERIFY_NOERROR(JsCreateObject(&functionLocation));
        CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(
            wrappedObj,
            "functionLocation",
            functionLocation));

        CHAKRA_VERIFY(CopyPropertyStringIfPresent(funcObj, "scriptId",
                                                  functionLocation));
        CHAKRA_VERIFY(CopyPropertyIfPresent(funcObj, "line", functionLocation,
                                            "lineNumber"));
        CHAKRA_VERIFY(CopyPropertyIfPresent(funcObj, "column", functionLocation,
                                            "columnNumber"));
      }
    }

    {
      JsValueRef location = nullptr;
      CHAKRA_VERIFY_NOERROR(JsCreateObject(&location));
      CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(wrappedObj,
                                                          "location",
                                                          location));

      CHAKRA_VERIFY(CopyPropertyStringIfPresent(callFrame, "scriptId",
                                                location));
      CHAKRA_VERIFY(CopyPropertyIfPresent(callFrame, "line", location,
                                          "lineNumber"));
      CHAKRA_VERIFY(CopyPropertyIfPresent(callFrame, "column", location,
                                          "columnNumber"));
    }

    {
      int index = 0;
      CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(callFrame, "index",
                                                            &index));

      JsValueRef stackProperties = nullptr;
      CHAKRA_VERIFY_NOERROR(JsDiagGetStackProperties(index, &stackProperties));

      {
        JsValueRef scopeChain = nullptr;
        CHAKRA_VERIFY_NOERROR(JsCreateArray(0, &scopeChain));
        CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(wrappedObj,
                                                            "scopeChain",
                                                            scopeChain));
        int nextIndex = 0;

        {
          bool hasProperty = false;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                              "locals",
                                                              &hasProperty));

          if (hasProperty) {
            JsValueRef localObj = nullptr;
            CHAKRA_VERIFY_NOERROR(JsCreateObject(&localObj));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetIndexedProperty(
                scopeChain, nextIndex++, localObj));

            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                localObj, "type", "local"));

            JsValueRef object = nullptr;
            CHAKRA_VERIFY_NOERROR(JsCreateObject(&object));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(localObj,
                                                                "object",
                                                                object));

            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "type", "object"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "className", "Object"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "description", "Object"));            
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "objectId",
                GetObjectIdForFrameProp(index, "locals").c_str()));
          }
        }

        {
          bool hasProperty = false;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                              "scopes",
                                                              &hasProperty));

          if (hasProperty) {
            JsValueRef scopes = nullptr;
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetProperty(stackProperties,
                                                                "scopes",
                                                                &scopes));

            int length = 0;
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(scopes,
                                                                   "length",
                                                                   &length));

            for (int i = 0; i < length; i++) {
              JsValueRef index = nullptr;
              CHAKRA_VERIFY_NOERROR(JsIntToNumber(i, &index));

              JsValueRef scope = nullptr;
              CHAKRA_VERIFY_NOERROR(JsGetIndexedProperty(scopes, index,
                                                         &scope));

              JsValueRef scopeObj = nullptr;
              CHAKRA_VERIFY_NOERROR(JsCreateObject(&scopeObj));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetIndexedProperty(
                scopeChain, nextIndex++, scopeObj));

              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                scopeObj, "type", "closure"));

              JsValueRef object = nullptr;
              CHAKRA_VERIFY_NOERROR(JsCreateObject(&object));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(scopeObj,
                "object",
                object));

              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "type", "object"));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "className", "Object"));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "description", "Object"));

              int handle = 0;
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(scope,
                                                                     "handle",
                                                                     &handle));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                  object, "objectId", GetObjectIdForHandle(handle).c_str()));
            }
          }
        }

        {
          bool hasProperty = false;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                              "globals",
                                                              &hasProperty));

          if (hasProperty) {
            JsValueRef globalObj = nullptr;
            CHAKRA_VERIFY_NOERROR(JsCreateObject(&globalObj));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetIndexedProperty(
                scopeChain, nextIndex++, globalObj));

            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                globalObj, "type", "global"));

            JsValueRef object = nullptr;
            CHAKRA_VERIFY_NOERROR(JsCreateObject(&object));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(globalObj,
                                                                "object",
                                                                object));

            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "type", "object"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "className", "global"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "description", "global"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
                object, "objectId",
                GetObjectIdForFrameProp(index, "globals").c_str()));
          }
        }
      }

      {
        bool hasProperty = false;
        CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                            "thisObject",
                                                            &hasProperty));

        if (hasProperty)
        {
          JsValueRef thisObject = nullptr;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetProperty(stackProperties,
                                                              "thisObject",
                                                              &thisObject));

          JsValueRef thisObj = nullptr;
          WrapObject(thisObject, &thisObj);
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(wrappedObj,
                                                              "this",
                                                              thisObj));
        }
      }

      {
        bool hasProperty = false;
        CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                            "returnValue",
                                                            &hasProperty));

        if (hasProperty)
        {
          JsValueRef returnObj = nullptr;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetProperty(stackProperties,
                                                              "returnValue",
                                                              &returnObj));

          JsValueRef returnValue = nullptr;
          WrapObject(returnObj, &returnValue);
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(wrappedObj,
                                                              "returnValue",
                                                              returnValue));
        }
      }
    }

    return v8::Utils::ToLocal(static_cast<v8::Object*>(wrappedObj));
  }

  v8::Local<v8::Value> InspectorHelpers::WrapRuntimeException(
      JsValueRef eventData) {
    bool hasProperty = false;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(eventData,
                                                        "exception",
                                                        &hasProperty));

    if (!hasProperty) {
      return v8::Local<v8::Value>();
    }

    JsValueRef exceptionProperty = nullptr;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetProperty(eventData, "exception",
      &exceptionProperty));

    return WrapException(exceptionProperty);
  }

  v8::Local<v8::Value> InspectorHelpers::WrapPropertiesArray(JsValueRef properties) {
    JsValueRef propsArray = nullptr;
    CHAKRA_VERIFY_NOERROR(JsCreateArray(0, &propsArray));
    int nextIndex = 0;

    CHAKRA_VERIFY(properties != nullptr);

    int length = 0;
    CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::GetIntProperty(properties, "length", &length));

    for (int i = 0; i < length; i++) {
      JsValueRef index = nullptr;
      CHAKRA_VERIFY_NOERROR(JsIntToNumber(i, &index));

      JsValueRef propValue = nullptr;
      CHAKRA_VERIFY_NOERROR(JsGetIndexedProperty(properties, index, &propValue));

      JsValueRef wrappedProp = nullptr;
      InspectorHelpers::WrapProperty(propValue, &wrappedProp);
      InspectorHelpers::SetIndexedProperty(propsArray, nextIndex++, wrappedProp);
    }

    return v8::Utils::ToLocal(static_cast<v8::Value*>(propsArray));
  }

  v8::Local<v8::Value> InspectorHelpers::GetWrappedProperties(int handle) {
    JsValueRef diagProperties = nullptr;
    JsErrorCode err = JsDiagGetProperties(handle, 0, 5000, &diagProperties);
    if (err == JsErrorDiagInvalidHandle) {
      // The handle is no longer valid, this is likely due to a pending
      // request that wasn't serviced before the last continuation.
      return v8::Local<v8::Value>();
    }
    CHAKRA_VERIFY_NOERROR(err);

    JsValueRef allPropsArray = nullptr;
    CHAKRA_VERIFY_NOERROR(JsCreateArray(0, &allPropsArray));

    JsValueRef propertiesArray = nullptr;
    CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::GetProperty(
        diagProperties, "properties", &propertiesArray));
    ArrayConcat(allPropsArray, propertiesArray);

    JsValueRef debuggerOnlyPropertiesArray = nullptr;
    CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::GetProperty(
        diagProperties, "debuggerOnlyProperties",
        &debuggerOnlyPropertiesArray));
    ArrayConcat(allPropsArray, debuggerOnlyPropertiesArray);

    return jsrt::InspectorHelpers::WrapPropertiesArray(allPropsArray);
  }

  v8::Local<v8::Value> InspectorHelpers::GetWrappedStackLocals(JsValueRef stackProperties) {
    JsValueRef localsArray = nullptr;
    CHAKRA_VERIFY_NOERROR(JsCreateArray(0, &localsArray));

    bool hasProp = false;
    CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::HasProperty(stackProperties, "exception", &hasProp));

    if (hasProp) {
      JsValueRef exception = nullptr;
      CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::GetProperty(stackProperties, "exception", &exception));
      InspectorHelpers::ArrayConcat(localsArray, exception);
    }

    CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::HasProperty(stackProperties, "arguments", &hasProp));

    if (hasProp) {
      JsValueRef arguments = nullptr;
      CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::GetProperty(stackProperties, "arguments", &arguments));
      InspectorHelpers::ArrayConcat(localsArray, arguments);
    }

    CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::HasProperty(stackProperties, "functionCallsReturn", &hasProp));

    if (hasProp) {
      JsValueRef functionCallsReturn = nullptr;
      CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::GetProperty(stackProperties, "functionCallsReturn", &functionCallsReturn));
      InspectorHelpers::ArrayConcat(localsArray, functionCallsReturn);
    }

    CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::HasProperty(stackProperties, "locals", &hasProp));

    if (hasProp) {
      JsValueRef locals = nullptr;
      CHAKRA_VERIFY_NOERROR(jsrt::InspectorHelpers::GetProperty(stackProperties, "locals", &locals));
      InspectorHelpers::ArrayConcat(localsArray, locals);
    }

    return jsrt::InspectorHelpers::WrapPropertiesArray(localsArray);
  }

  v8::Local<v8::Value> InspectorHelpers::EvaluateOnCallFrame(
      int ordinal, JsValueRef expression, bool returnByValue, bool* isError) {
    CHAKRA_VERIFY(ordinal >= 0);

    if (isError != nullptr) {
      *isError = false;
    }

    JsValueRef evalResult = nullptr;
    JsErrorCode err = JsDiagEvaluate(expression, ordinal,
                                     JsParseScriptAttributeNone,
                                     returnByValue, &evalResult);

    if (err == JsErrorScriptException) {
      if (isError != nullptr) {
        *isError = true;
      }

      return WrapException(evalResult);
    }
    else if (err == JsErrorDiagNotAtBreak) {
      return v8::Local<v8::Value>();
    }

    CHAKRA_VERIFY_NOERROR(err);
    return WrapEvaluateObject(evalResult);
  }

  v8::Local<v8::Value> InspectorHelpers::EvaluateOnCallFrame(
      JsValueRef callFrame, JsValueRef expression, bool returnByValue,
      bool* isError) {
    CHAKRA_VERIFY(callFrame != nullptr);

    int ordinal = 0;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(callFrame, "index", &ordinal));

    return EvaluateOnCallFrame(ordinal, expression, returnByValue, isError);
  }

  v8::Local<v8::Value> InspectorHelpers::GetScriptSource(unsigned int scriptId) {
    JsValueRef scriptSource = nullptr;
    CHAKRA_VERIFY_NOERROR(JsDiagGetSource(scriptId, &scriptSource));

    JsValueRef sourceStr = nullptr;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetProperty(scriptSource, "source", &sourceStr));

    return v8::Utils::ToLocal(static_cast<v8::Value*>(sourceStr));
  }

  JsRuntimeHandle InspectorHelpers::GetRuntimeFromIsolate(
      v8::Isolate *isolate) {
    return IsolateShim::FromIsolate(isolate)->GetRuntimeHandle();
  }

  void InspectorHelpers::WrapObject(JsValueRef obj, JsValueRef* wrappedObj) {
    JsValueRef value = nullptr;
    CHAKRA_VERIFY_NOERROR(JsCreateObject(&value));

    CHAKRA_VERIFY(CopyPropertyIfPresent(obj, "type", value));

    CopyPropertyIfPresent(obj, "className", value);
    
    bool hasValue = CopyPropertyIfPresent(obj, "value", value);
    bool hasDisplay = CopyPropertyIfPresent(obj, "display", value, "description");

    // A description is required for values to be shown in the debugger.
    if (hasValue && !hasDisplay) {
      hasDisplay = CopyPropertyStringIfPresent(obj, "value", value, "description");
    }

    CHAKRA_VERIFY(hasDisplay);

    int handle = 0;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(obj, "handle", &handle));
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(value, "objectId", GetObjectIdForHandle(handle).c_str()));

    *wrappedObj = value;
  }

  void InspectorHelpers::WrapProperty(JsValueRef propValue, JsValueRef* wrappedProperty) {
    JsValueRef wrappedObj = nullptr;
    CHAKRA_VERIFY_NOERROR(JsCreateObject(&wrappedObj));

    CHAKRA_VERIFY(CopyPropertyIfPresent(propValue, "name", wrappedObj));
    
    JsValueRef value = nullptr;
    WrapObject(propValue, &value);
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(wrappedObj, "value", value));

    int propertyAttributes = 0;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(propValue, "propertyAttributes", &propertyAttributes));
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetBoolProperty(wrappedObj, "writable", (propertyAttributes & JsrtDebugPropertyAttributeReadOnly) == 0));
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetBoolProperty(wrappedObj, "configurable", false));
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetBoolProperty(wrappedObj, "enumerable", false));

    *wrappedProperty = wrappedObj;
  }

  v8::Local<v8::Value> InspectorHelpers::WrapException(JsValueRef exception) {
    JsValueRef exceptionVal = nullptr;
    CHAKRA_VERIFY_NOERROR(JsCreateObject(&exceptionVal));

    CHAKRA_VERIFY(CopyPropertyIfPresent(exception, "type", exceptionVal));
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(exceptionVal,
                                                              "subtype",
                                                              "error"));
    CHAKRA_VERIFY(CopyPropertyIfPresent(exception, "className",
                                        exceptionVal));
    CHAKRA_VERIFY(CopyPropertyIfPresent(exception, "display",
                                        exceptionVal, "description"));

    int handle = 0;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(exception,
                                                           "handle",
                                                           &handle));
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetStringProperty(
        exceptionVal, "objectId", GetObjectIdForHandle(handle).c_str()));

    return v8::Utils::ToLocal(static_cast<v8::Value*>(exceptionVal));
  }

}  // namespace jsrt
