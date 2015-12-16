// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef NODE_LIBVIRT_H
#define NODE_LIBVIRT_H

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#include <nan.h>
using namespace v8;
using namespace node;

#define NODE_LIBVIRT_VERSION "v0.1.0"

#define NLV_WARN_UNFINISHED(Method)   \
  fprintf(stderr, "WARNING: " #Method " is still synchronous and needs to be ported\n");

#ifndef LIBVIR_CHECK_VERSION
# define LIBVIR_CHECK_VERSION(major, minor, micro) false
#endif

#endif  // NODE_LIBVIRT_H
