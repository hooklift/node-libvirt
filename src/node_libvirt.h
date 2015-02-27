// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NODE_LIBVIRT_H_
#define SRC_NODE_LIBVIRT_H_

#include <node.h>
#include <nan.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#define NODE_LIBVIRT_VERSION "v0.1.0"

using namespace v8;
using namespace node;

#define LIBVIRT_THROW_EXCEPTION(err)                                        \
        v8::Local<v8::Value> exception = v8::Exception::Error(              \
        v8::String::New(err));                                              \
        ThrowException(exception);

#define TO_V8_ARRAY(size,items)                                     \
        Local<Array> v8Array = Array::New(size);                    \
        for(int i = 0; i < size; i++) {                             \
            v8Array->Set(Integer::New(i), String::New(items[i]));   \
            free(items[i]);                                         \
        }                                                           \
        free(items);                                                \
        return scope.Close(v8Array);

namespace NodeLibvirt {
    const char* ToCString(const v8::String::Utf8Value& value);
    //Handle<Value> ToV8Array(int size, char **items);
}

#endif  // SRC_NODE_LIBVIRT_H_
