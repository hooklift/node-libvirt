// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_INTERFACE_H_
#define SRC_INTERFACE_H_

#include "nlv_object.h"
#include "nlv_async_worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

struct InterfaceCleanupHandler {
  static int cleanup(virInterfacePtr handle) {
    return virInterfaceFree(handle);
  }
};

class Interface : public NLVObject<virInterfacePtr, InterfaceCleanupHandler>
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virInterfacePtr handle);

private:
  static Persistent<Function> constructor;
  friend class Hypervisor;

private:
  explicit Interface(virInterfacePtr handle);

  // HYPERVISOR METHODS
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByMacAddress);
  static NAN_METHOD(Define);

  // METHODS
  static NAN_METHOD(Start);
  static NAN_METHOD(Stop);
  static NAN_METHOD(Undefine);
  static NAN_METHOD(IsActive);
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetMacAddress);
  static NAN_METHOD(ToXml);

private:
  // HYPERVISOR METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByName, Interface, virConnectPtr, virInterfacePtr);
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByMacAddress, Interface, virConnectPtr, virInterfacePtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Define, Interface, virConnectPtr, virInterfacePtr);

  // WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Start, virInterfacePtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Stop, virInterfacePtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, virInterfacePtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsActive, virInterfacePtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetName, virInterfacePtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetMacAddress, virInterfacePtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, virInterfacePtr, std::string);


};

}  // namespace NodeLibvirt

#endif  // SRC_INTERFACE_H_
