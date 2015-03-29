// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NODE_DEVICE_H_
#define SRC_NODE_DEVICE_H_

#include <nan.h>

namespace NodeLibvirt {

class NodeDevice : public ObjectWrap
{
public:
  static void Initialize();

private:
  static Persistent<FunctionTemplate> constructor_template;
  virNodeDevicePtr device_;

protected:
  static NAN_METHOD(Create);
  static NAN_METHOD(Destroy);
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(Detach);
  static NAN_METHOD(Reattach);
  static NAN_METHOD(Reset);
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetParentName);
  static NAN_METHOD(ToXml);
  static NAN_METHOD(GetCapabilities);


};

}  //namespace NodeLibvirt

#endif  // SRC_NODE_DEVICE_H

