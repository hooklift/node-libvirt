// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NODE_LIBVIRT_H_
#define SRC_NODE_LIBVIRT_H_

#include <node.h>
#include <node_events.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#define NODE_LIBVIRT_VERSION "v0.0.1"
#define LIBVIRT_VERSION "0.7.0"

#define LIBVIRT_THROW_EXCEPTION(err)                                        \
        v8::Local<v8::Value> exception = v8::Exception::TypeError(          \
        v8::String::New(err));                                              \
        ThrowException(exception);                                          

using namespace v8;
using namespace node;

namespace NodeLibvirt {
    const char* ToCString(const v8::String::Utf8Value& value);
}

#endif  // SRC_NODE_LIBVIRT_H_
