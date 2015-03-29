// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_INTERFACE_H_
#define SRC_INTERFACE_H_

#include <nan.h>

namespace NodeLibvirt {

class Interface : public ObjectWrap
{
public:
  static void Initialize();

private:
  explicit Interface();
  friend class Hypervisor;

  static Persistent<FunctionTemplate> constructor_template;
  virInterfacePtr interface_;

private:
  static NAN_METHOD(Start);
  static NAN_METHOD(Stop);
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByMacAddress);
  static NAN_METHOD(Define);
  static NAN_METHOD(Undefine);
  static NAN_METHOD(GetName);
  static NAN_METHOD(IsActive);
  static NAN_METHOD(ToXml);
  static NAN_METHOD(GetMacAddress);

};

}  // namespace NodeLibvirt

#endif  // SRC_INTERFACE_H_

