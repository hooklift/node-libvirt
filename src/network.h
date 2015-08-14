// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NETWORK_H_
#define SRC_NETWORK_H_

#include "node_libvirt.h"

#include "worker.h"
#include "worker_macros.h"

#include "nlv_async_worker.h"

namespace NodeLibvirt {

class Network : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance2(virNetworkPtr handle);
  virtual ~Network();

private:
  explicit Network(virNetworkPtr handle) : handle_(handle) {}
  static Persistent<Function> constructor;
  virNetworkPtr handle_;

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
  NLV_LOOKUP_BY_VALUE_WORKER2(LookupByName, Network, virConnectPtr, virNetworkPtr);
  NLV_LOOKUP_BY_VALUE_WORKER2(LookupByUUID, Network, virConnectPtr, virNetworkPtr);
  NLV_LOOKUP_BY_VALUE_WORKER2(Define, Network, virConnectPtr, virNetworkPtr);
  NLV_LOOKUP_BY_VALUE_WORKER2(Create, Network, virConnectPtr, virNetworkPtr);

  // METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER2(Destroy, virNetworkPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER2(Start, virNetworkPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER2(Undefine, virNetworkPtr, bool);

  // ACCESSORS/MUTATORS WORKERS
  NLV_PRIMITIVE_RETURN_WORKER2(GetName, virNetworkPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetUUID, virNetworkPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetAutostart, virNetworkPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER2(GetBridgeName, virNetworkPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(IsActive, virNetworkPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER2(IsPersistent, virNetworkPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER2(ToXml, virNetworkPtr, std::string);

  class SetAutostartWorker : public NLVPrimitiveReturnWorker<virNetworkPtr, bool> {
  public:
    SetAutostartWorker(NanCallback *callback, virNetworkPtr handle, bool autoStart)
      : NLVPrimitiveReturnWorker<virNetworkPtr, bool>(callback, handle), autoStart_(autoStart) {} \
    void Execute();
  private:
    bool autoStart_;
  };

};

}  // namespace NodeLibvirt

#endif  // SRC_NETWORK_H_
