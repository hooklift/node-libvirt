// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NODE_LIBVIRT_H_
#define SRC_NODE_LIBVIRT_H_

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#include <nan.h>
using namespace v8;
using namespace node;

#define NODE_LIBVIRT_VERSION "v0.1.0"
#define LIBVIRT_THROW_EXCEPTION(err)                                        \
        v8::Local<v8::Value> exception = v8::Exception::Error(              \
        v8::String::New(err));                                              \
        ThrowException(exception);

#endif  // SRC_NODE_LIBVIRT_H_
