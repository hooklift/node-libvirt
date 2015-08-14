// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NODE_LIBVIRT_H_
#define SRC_NODE_LIBVIRT_H_

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#include <nan.h>
using namespace v8;
using namespace node;

#define NODE_LIBVIRT_VERSION "v0.1.0"

#define NLV_WARN_UNFINISHED(Method)   \
  fprintf(stderr, "WARNING: " #Method " is still synchronous and needs to be ported\n");

#endif  // SRC_NODE_LIBVIRT_H_
