// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SRC_NODE_H_
#define SRC_NODE_H_

#include <v8.h>

namespace node {

// "origin" (normally file name) is for easier debug
// When there's problem, the origin is displayed so that you know where to fix

void Run(const char* jsString, int len = -1, const char* origin = NULL);
void Run(unsigned char* jsBytes, int len, const char* origin = NULL);
v8::Handle<v8::Script> Compile(unsigned char* jsBytes, int len, const char* origin = NULL);

#define NODE_PSYMBOL(s) \
v8::Persistent<v8::String>::New(v8::String::NewSymbol(s))

template <typename target_t>
void SetMethod(target_t obj, const char* name,
        v8::InvocationCallback callback)
{
    obj->Set(v8::String::NewSymbol(name),
        v8::FunctionTemplate::New(callback)->GetFunction());
}

template <typename target_t>
void SetPrototypeMethod(target_t target,
        const char* name, v8::InvocationCallback callback)
{
    v8::Local<v8::FunctionTemplate> templ = v8::FunctionTemplate::New(callback);
    target->PrototypeTemplate()->Set(v8::String::NewSymbol(name), templ);
}

// for backwards compatibility
#define NODE_SET_METHOD node::SetMethod
#define NODE_SET_PROTOTYPE_METHOD node::SetPrototypeMethod

enum encoding {ASCII, UTF8, BASE64, UCS2, BINARY, HEX};
enum encoding ParseEncoding(v8::Handle<v8::Value> encoding_v,
                            enum encoding _default = BINARY);

v8::Local<v8::Value> Encode(const void *buf, size_t len,
                            enum encoding encoding = BINARY);

// Returns -1 if the handle was not valid for decoding
ssize_t DecodeBytes(v8::Handle<v8::Value>,
                    enum encoding encoding = BINARY);

// returns bytes written.
ssize_t DecodeWrite(char *buf,
                    size_t buflen,
                    v8::Handle<v8::Value>,
                    enum encoding encoding = BINARY);

/**
* Call this when your constructor is invoked as a regular function, e.g.
* Buffer(10) instead of new Buffer(10).
* @param constructorTemplate Constructor template to instantiate from.
* @param args The arguments object passed to your constructor.
* @see v8::Arguments::IsConstructCall
*/
v8::Handle<v8::Value> FromConstructorTemplate(
    v8::Persistent<v8::FunctionTemplate>& constructorTemplate,
    const v8::Arguments& args);

v8::Handle<v8::Value> ThrowError(const char* msg);

v8::Handle<v8::Value> ThrowTypeError(const char* msg);

v8::Handle<v8::Value> ThrowRangeError(const char* msg);

}
#endif  // SRC_NODE_H_
