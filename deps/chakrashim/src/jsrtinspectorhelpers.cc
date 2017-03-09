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

    JsValueRef sourceVal;
    err = JsGetProperty(sourceObj, propId, &sourceVal);
    if (err != JsNoError) {
      return err;
    }

    JsValueRef destVal;
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
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::CopyPropertyIfPresent(
        sourceObj, sourceName, destObj, destName, &wasCopied));

    return wasCopied;
  }

  static bool CopyPropertyStringIfPresent(JsValueRef sourceObj,
                                          const char *sourceName,
                                          JsValueRef destObj,
                                          const char *destName = nullptr) {
    bool wasCopied;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::CopyPropertyStringIfPresent(
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

    JsValueRef propVal;
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

    JsValueRef propVal;
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

  JsErrorCode InspectorHelpers::SetProperty(JsValueRef obj, const char *name,
                                            JsValueRef value) {
    JsErrorCode err = JsNoError;

    JsPropertyIdRef propId;
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

  JsErrorCode InspectorHelpers::SetPropertyString(JsValueRef obj,
                                                  const char *name,
                                                  const char *value) {
    JsErrorCode err = JsNoError;

    JsValueRef stringVal;
    err = JsCreateString(value, strlen(value), &stringVal);
    if (err != JsNoError) {
      return err;
    }

    JsPropertyIdRef propId;
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

    JsValueRef indexVal;
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

  JsErrorCode InspectorHelpers::CopyPropertyIfPresent(
      JsValueRef sourceObj,
      const char *sourceName,
      JsValueRef destObj,
      const char *destName,
      bool *wasCopied) {
    return CopyPropertyImpl(sourceObj, sourceName, destObj, destName,
                            static_cast<ConvertFunc>(nullptr), wasCopied);
  }

  JsErrorCode InspectorHelpers::CopyPropertyStringIfPresent(
      JsValueRef sourceObj,
      const char *sourceName,
      JsValueRef destObj,
      const char *destName,
      bool *wasCopied) {
    return CopyPropertyImpl(sourceObj, sourceName, destObj, destName,
                            &JsConvertValueToString, wasCopied);
  }

  v8::Local<v8::Value> InspectorHelpers::WrapEvaluateObject(
      JsValueRef sourceObject) {
    JsErrorCode errorCode = JsNoError;

    JsValueRef resultObj;
    errorCode = JsCreateObject(&resultObj);

    CHAKRA_ASSERT(CopyPropertyIfPresent(sourceObject, "type", resultObj));
    CHAKRA_ASSERT(CopyPropertyIfPresent(sourceObject, "value", resultObj));

    return v8::Utils::ToLocal(static_cast<v8::Value*>(resultObj));
  }

  v8::Local<v8::Object> InspectorHelpers::WrapCallFrameDetails(
      JsValueRef callFrame) {
    JsValueRef wrappedObj;
    CHAKRA_VERIFY_NOERROR(JsCreateObject(&wrappedObj));

    {
      int functionHandle;
      CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(callFrame,
                                                             "functionHandle",
                                                             &functionHandle));

      JsValueRef funcObj;
      CHAKRA_VERIFY_NOERROR(JsDiagGetObjectFromHandle(functionHandle,
                                                      &funcObj));

      CHAKRA_VERIFY(CopyPropertyIfPresent(funcObj, "name", wrappedObj,
                                          "functionName"));

      {
        JsValueRef functionLocation;
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
      JsValueRef location;
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
      int index;
      CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(callFrame, "index",
                                                            &index));

      JsValueRef stackProperties;
      CHAKRA_VERIFY_NOERROR(JsDiagGetStackProperties(index, &stackProperties));

      {
        JsValueRef scopeChain;
        CHAKRA_VERIFY_NOERROR(JsCreateArray(0, &scopeChain));
        CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(wrappedObj,
                                                            "scopeChain",
                                                            scopeChain));
        int nextIndex = 0;

        {
          bool hasProperty;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                              "locals",
                                                              &hasProperty));

          if (hasProperty) {
            JsValueRef localObj;
            CHAKRA_VERIFY_NOERROR(JsCreateObject(&localObj));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetIndexedProperty(
                scopeChain, nextIndex++, localObj));

            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                localObj, "type", "local"));

            JsValueRef object;
            CHAKRA_VERIFY_NOERROR(JsCreateObject(&object));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(localObj,
                                                                "object",
                                                                object));

            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "type", "object"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "className", "Object"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "description", "Object"));            
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "objectId",
                GetObjectIdForFrameProp(index, "locals").c_str()));
          }
        }

        {
          bool hasProperty;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                              "scopes",
                                                              &hasProperty));

          if (hasProperty) {
            JsValueRef scopes;
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetProperty(stackProperties,
                                                                "scopes",
                                                                &scopes));

            int length;
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(scopes,
                                                                   "length",
                                                                   &length));

            for (int i = 0; i < length; i++) {
              JsValueRef index;
              CHAKRA_VERIFY_NOERROR(JsIntToNumber(i, &index));

              JsValueRef scope;
              CHAKRA_VERIFY_NOERROR(JsGetIndexedProperty(scopes, index,
                                                         &scope));

              JsValueRef scopeObj;
              CHAKRA_VERIFY_NOERROR(JsCreateObject(&scopeObj));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetIndexedProperty(
                scopeChain, nextIndex++, scopeObj));

              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                scopeObj, "type", "closure"));

              JsValueRef object;
              CHAKRA_VERIFY_NOERROR(JsCreateObject(&object));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(scopeObj,
                "object",
                object));

              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "type", "object"));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "className", "Object"));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "description", "Object"));

              int handle;
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(scope,
                                                                     "handle",
                                                                     &handle));
              CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                  object, "objectId", GetObjectIdForHandle(handle).c_str()));
            }
          }
        }

        {
          bool hasProperty;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                              "globals",
                                                              &hasProperty));

          if (hasProperty) {
            JsValueRef globalObj;
            CHAKRA_VERIFY_NOERROR(JsCreateObject(&globalObj));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetIndexedProperty(
                scopeChain, nextIndex++, globalObj));

            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                globalObj, "type", "global"));

            JsValueRef object;
            CHAKRA_VERIFY_NOERROR(JsCreateObject(&object));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(globalObj,
                                                                "object",
                                                                object));

            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "type", "object"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "className", "global"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "description", "global"));
            CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
                object, "objectId",
                GetObjectIdForFrameProp(index, "globals").c_str()));
          }
        }
      }

      {
        bool hasProperty;
        CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                            "thisObject",
                                                            &hasProperty));

        if (hasProperty)
        {
          JsValueRef thisObject;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetProperty(stackProperties,
                                                              "thisObject",
                                                              &thisObject));

          JsValueRef thisObj;
          CHAKRA_VERIFY_NOERROR(JsCreateObject(&thisObj));
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(wrappedObj,
                                                              "this",
                                                              thisObj));

          CHAKRA_VERIFY(CopyPropertyIfPresent(thisObject, "type", thisObj));

          // These are optional and situational, don't require them.
          CopyPropertyIfPresent(thisObject, "className", thisObj);
          CopyPropertyIfPresent(thisObject, "value", thisObj);

          CHAKRA_VERIFY(CopyPropertyIfPresent(thisObject, "display", thisObj,
                                              "description"));

          int handle;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(thisObject,
                                                                 "handle",
                                                                 &handle));
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
              thisObj, "objectId", GetObjectIdForHandle(handle).c_str()));
        }
      }

      {
        bool hasProperty;
        CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(stackProperties,
                                                            "returnValue",
                                                            &hasProperty));

        if (hasProperty)
        {
          JsValueRef returnObj;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetProperty(stackProperties,
                                                              "returnValue",
                                                              &returnObj));

          JsValueRef returnValue;
          CHAKRA_VERIFY_NOERROR(JsCreateObject(&returnValue));
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetProperty(wrappedObj,
                                                              "returnValue",
                                                              returnValue));

          CHAKRA_VERIFY(CopyPropertyIfPresent(returnObj, "type", returnValue));
          CHAKRA_VERIFY(CopyPropertyIfPresent(returnObj, "className",
                                              returnValue));
          CHAKRA_VERIFY(CopyPropertyIfPresent(returnObj, "display",
                                              returnValue, "description"));

          int handle;
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(returnObj,
                                                                 "handle",
                                                                 &handle));
          CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
              returnValue, "objectId", GetObjectIdForHandle(handle).c_str()));
        }
      }
    }

    return v8::Utils::ToLocal(static_cast<v8::Object*>(wrappedObj));
  }

  v8::Local<v8::Value> InspectorHelpers::WrapRuntimeException(
      JsValueRef eventData) {
    bool hasProperty;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::HasProperty(eventData,
                                                        "exception",
                                                        &hasProperty));

    if (!hasProperty) {
      return v8::Local<v8::Value>();
    }

    JsValueRef exception;
    CHAKRA_VERIFY_NOERROR(JsCreateObject(&exception));

    JsValueRef exceptionProperty;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetProperty(eventData, "exception",
                                                        &exceptionProperty));

    CHAKRA_VERIFY(CopyPropertyIfPresent(exceptionProperty, "type", exception));
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(exception,
                                                              "subtype",
                                                              "error"));
    CHAKRA_VERIFY(CopyPropertyIfPresent(exceptionProperty, "className",
                                        exception));
    CHAKRA_VERIFY(CopyPropertyIfPresent(exceptionProperty, "display",
                                        exception, "description"));

    int handle;
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::GetIntProperty(exceptionProperty,
                                                           "handle",
                                                           &handle));
    CHAKRA_VERIFY_NOERROR(InspectorHelpers::SetPropertyString(
        exception, "objectId", GetObjectIdForHandle(handle).c_str()));

    return v8::Utils::ToLocal(static_cast<v8::Value*>(exception));
  }

  JsRuntimeHandle InspectorHelpers::GetRuntimeFromIsolate(
      v8::Isolate *isolate) {
    return IsolateShim::FromIsolate(isolate)->GetRuntimeHandle();
  }

}  // namespace jsrt
