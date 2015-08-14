// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_SECRET_H_
#define SRC_SECRET_H_

#include "node_libvirt.h"

#include "nlv_async_worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class Secret : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virSecretPtr handle);
  virtual ~Secret();

private:
  explicit Secret(virSecretPtr handle) : handle_(handle) {}
  static Persistent<Function> constructor;
  virSecretPtr handle_;

  friend class Hypervisor;

private:
  // HYPERVISOR METHODS
  static NAN_METHOD(LookupByUsage);
  static NAN_METHOD(LookupByUUID);
  static NAN_METHOD(Define);

  // ACTIONS
  static NAN_METHOD(Undefine);

  // ACCESSORS/MUTATORS
  static NAN_METHOD(GetUUID);
  static NAN_METHOD(GetUsageId);
  static NAN_METHOD(GetUsageType);
  static NAN_METHOD(GetValue);
  static NAN_METHOD(SetValue);
  static NAN_METHOD(ToXml);

private:
  // HYPERVISOR METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByUUID, Secret, virConnectPtr, virSecretPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Define, Secret, virConnectPtr, virSecretPtr);

  class LookupByUsageWorker : public NLVLookupInstanceByValueWorker<Secret, virConnectPtr, virSecretPtr> {
  public:
    LookupByUsageWorker(NanCallback *callback, virConnectPtr handle, const std::string &usageId, int usageType) \
      : NLVLookupInstanceByValueWorker<Secret, virConnectPtr, virSecretPtr>(callback, handle, usageId), usageType_(usageType) {}
    void Execute();
  private:
    int usageType_;
  };

  // ACTION WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, virSecretPtr, bool);

  // ACCESSOR/MUTATOR WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(GetUUID, virSecretPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetUsageId, virSecretPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetUsageType, virSecretPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetValue, virSecretPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, virSecretPtr, std::string);

  class SetValueWorker : public NLVPrimitiveReturnWorker<virSecretPtr, bool> {
  public:
    SetValueWorker(NanCallback *callback, virSecretPtr handle, const std::string &value)
      : NLVPrimitiveReturnWorker<virSecretPtr, bool>(callback, handle), value_(value) {} \
    void Execute();
  private:
    std::string value_;
  };

};

}  // namespace NodeLibvirt

#endif  // SRC_SECRET_H_
