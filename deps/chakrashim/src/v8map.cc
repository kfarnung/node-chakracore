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

#include "v8chakra.h"

namespace v8 {

Local<Map> Map::New(Isolate* isolate) {
  // CHAKRA-TODO: Figure out what to do here
  //
  // kpathak: All these should be easy to implement via chakra_shim.js.
  CHAKRA_ASSERT(false);
  return Local<Map>();
}

MaybeLocal<Value> Map::Get(Local<Context>, Local<Value>) {
  // CHAKRA-TODO: Figure out what to do here
  CHAKRA_ASSERT(false);
  return MaybeLocal<Value>();
}

MaybeLocal<Map> Map::Set(Local<Context>, Local<Value>, Local<Value>) {
  // CHAKRA-TODO: Figure out what to do here
  CHAKRA_ASSERT(false);
  return MaybeLocal<Map>();
}

Maybe<bool> Map::Has(Local<Context>, Local<Value>) {
  // CHAKRA-TODO: Figure out what to do here
  CHAKRA_ASSERT(false);
  return Nothing<bool>();
}

Map* Map::Cast(Value* obj) {
  CHAKRA_ASSERT(obj->IsMap());
  return static_cast<Map*>(obj);
}

}  // namespace v8
