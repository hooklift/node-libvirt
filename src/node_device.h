// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NODE_DEVICE_H_
#define SRC_NODE_DEVICE_H_

#include "node_libvirt.h"

#include "worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class NodeDevice : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(const LibVirtHandle &handle);
  virtual ~NodeDevice();

private:
  explicit NodeDevice(virNodeDevicePtr handle) : handle_(handle) {}
  static Persistent<FunctionTemplate> constructor_template;
  virNodeDevicePtr handle_;

  friend class Hypervisor;

private:
  // HYPERVISOR METHODS
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(Create);

  // ACTIONS
  static NAN_METHOD(Destroy);
  static NAN_METHOD(Detach);
  static NAN_METHOD(Reattach);
  static NAN_METHOD(Reset);

  // ACCESSORS/MUTATORS
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetParentName);
  static NAN_METHOD(ToXml);
  static NAN_METHOD(GetCapabilities);

private:
  // HYPERVISOR METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(NodeDevice, LookupByName);
  NLV_LOOKUP_BY_VALUE_WORKER(NodeDevice, Create);

  // ACTION METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Destroy, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Detach, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Reattach, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Reset, bool);

  // ACCESSOR/MUTATOR METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(GetName, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetParentName, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, std::string);
  NLV_LIST_RETURN_WORKER(GetCapabilities, std::string, v8::String);

};

}  //namespace NodeLibvirt

#endif  // SRC_NODE_DEVICE_H

