// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef NODE_DEVICE_H
#define NODE_DEVICE_H

#include "nlv_object.h"
#include "nlv_async_worker.h"
#include "worker_macros.h"

#include "hypervisor.h"

namespace NLV {

struct NodeDeviceCleanupHandler {
  static int cleanup(virNodeDevicePtr handle) {
    return virNodeDeviceFree(handle);
  }
};

class NodeDevice : public NLVObject<virNodeDevicePtr, NodeDeviceCleanupHandler>
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virNodeDevicePtr handle);

private:
  explicit NodeDevice(virNodeDevicePtr handle);
  static Nan::Persistent<Function> constructor;
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
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByName, NodeDevice, Hypervisor, virNodeDevicePtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Create, NodeDevice, Hypervisor, virNodeDevicePtr);

  // ACTION METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Destroy, virNodeDevicePtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Detach, virNodeDevicePtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Reattach, virNodeDevicePtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Reset, virNodeDevicePtr, bool);

  // ACCESSOR/MUTATOR METHOD WORKERS
  NLV_STRING_RETURN_WORKER(GetName, virNodeDevicePtr, std::string);
  NLV_STRING_RETURN_WORKER(GetParentName, virNodeDevicePtr, std::string);
  NLV_STRING_RETURN_WORKER(ToXml, virNodeDevicePtr, std::string);
  NLV_STRING_LIST_RETURN_WORKER(GetCapabilities, virNodeDevicePtr, std::string, v8::String);

};

}  //namespace NLV

#endif  // NODE_DEVICE_H
