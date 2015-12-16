// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef ERROR_H
#define ERROR_H

#include "node_libvirt.h"

namespace NLV {

class Error : public Nan::ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Value> New(virErrorPtr error);

private:
  explicit Error(virErrorPtr error);
  static Nan::Persistent<Function> constructor;

  static NAN_GETTER(Getter);

  virErrorPtr error_;

};

} //namespace NLV

#endif // ERROR_H
