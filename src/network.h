// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef NETWORK_H
#define NETWORK_H

#include "nlv_object.h"
#include "nlv_async_worker.h"
#include "worker_macros.h"

#include "hypervisor.h"

namespace NLV {

struct NetworkCleanupHandler {
  static int cleanup(virNetworkPtr handle) {
    return virNetworkFree(handle);
  }
};

class Network : public NLVObject<virNetworkPtr, NetworkCleanupHandler>
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virNetworkPtr handle);

private:
  explicit Network(virNetworkPtr handle);
  static Nan::Persistent<Function> constructor;
  friend class Hypervisor;

protected:
  // HYPERVISOR METHODS
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByUUID);
  static NAN_METHOD(Define);
  static NAN_METHOD(Create);

  // ACTIONS
  static NAN_METHOD(Destroy);
  static NAN_METHOD(Start);
  static NAN_METHOD(Undefine);

  // ACCESSORS/MUTATORS
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetUUID);
  static NAN_METHOD(GetAutostart);
  static NAN_METHOD(SetAutostart);
  static NAN_METHOD(GetBridgeName);
  static NAN_METHOD(IsActive);
  static NAN_METHOD(IsPersistent);
  static NAN_METHOD(ToXml);

private:
  // HYPERVISOR METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByName, Network, Hypervisor, virNetworkPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByUUID, Network, Hypervisor, virNetworkPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Define, Network, Hypervisor, virNetworkPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Create, Network, Hypervisor, virNetworkPtr);

  // METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Destroy, virNetworkPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Start, virNetworkPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, virNetworkPtr, bool);

  // ACCESSORS/MUTATORS WORKERS
  NLV_STRING_RETURN_WORKER(GetName, virNetworkPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetUUID, virNetworkPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetAutostart, virNetworkPtr, bool);
  NLV_STRING_RETURN_WORKER(GetBridgeName, virNetworkPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(IsActive, virNetworkPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsPersistent, virNetworkPtr, bool);
  NLV_STRING_RETURN_WORKER(ToXml, virNetworkPtr, std::string);

  class SetAutostartWorker : public NLVPrimitiveReturnWorker<virNetworkPtr, bool> {
  public:
    SetAutostartWorker(Nan::Callback *callback, virNetworkPtr handle, bool autoStart)
      : NLVPrimitiveReturnWorker<virNetworkPtr, bool>(callback, handle), autoStart_(autoStart) {} \
    void Execute();
  private:
    bool autoStart_;
  };

};

}  // namespace NLV

#endif  // NETWORK_H
