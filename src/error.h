// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_ERROR_H_
#define SRC_ERROR_H_

#include <nan.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

using namespace v8;
using namespace node;

namespace NodeLibvirt {

class Error : public ObjectWrap
{
public:
  static void Initialize();
  static Handle<Value> New(virErrorPtr error);

private:
  explicit Error(virErrorPtr error);
  static Persistent<FunctionTemplate> constructor_template;

  static NAN_GETTER(Getter);

  virErrorPtr error_;

};

} //namespace NodeLibvirt

#endif // SRC_ERROR_H_
