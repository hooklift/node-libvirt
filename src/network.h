// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NETWORK_H_
#define SRC_NETWORK_H_

#include "node_libvirt.h"

namespace NodeLibvirt {

class Network : public ObjectWrap {
public:
  static void Initialize();

private:
  static Persistent<FunctionTemplate> constructor_template;
  virNetworkPtr network_;

  friend class Hypervisor;

protected:
  static NAN_METHOD(Create);
  static NAN_METHOD(Start);
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByUUID);
  static NAN_METHOD(Define);
  static NAN_METHOD(Undefine);
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetUUID);
  static NAN_METHOD(GetAutostart);
  static NAN_METHOD(SetAutostart);
  static NAN_METHOD(IsActive);
  static NAN_METHOD(IsPersistent);
  static NAN_METHOD(ToXml);
  static NAN_METHOD(GetBridgeName);
  static NAN_METHOD(Destroy);

};

}  // namespace NodeLibvirt

#endif  // SRC_NETWORK_H_

