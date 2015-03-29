// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_SECRET_H_
#define SRC_SECRET_H_

#include <nan.h>

namespace NodeLibvirt {

class Secret : public ObjectWrap
{
public:
  static void Initialize();

private:
  static Persistent<FunctionTemplate> constructor_template;
  virSecretPtr secret_;

private:
  static NAN_METHOD(Define);
  static NAN_METHOD(Undefine);
  static NAN_METHOD(LookupByUsage);
  static NAN_METHOD(LookupByUUID);
  static NAN_METHOD(GetUUID);
  static NAN_METHOD(GetUsageId);
  static NAN_METHOD(GetUsageType);
  static NAN_METHOD(GetValue);
  static NAN_METHOD(SetValue);
  static NAN_METHOD(ToXml);

};

}  // namespace NodeLibvirt

#endif  // SRC_SECRET_H_

