// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SECRET_H
#define SECRET_H

#include "nlv_object.h"
#include "nlv_async_worker.h"
#include "worker_macros.h"

#include "hypervisor.h"

namespace NLV {

struct SecretCleanupHandler {
  static int cleanup(virSecretPtr handle) {
    return virSecretFree(handle);
  }
};

class Secret : public NLVObject<virSecretPtr, SecretCleanupHandler>
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virSecretPtr handle);

private:
  explicit Secret(virSecretPtr handle);
  static Nan::Persistent<Function> constructor;
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
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByUUID, Secret, Hypervisor, virSecretPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Define, Secret, Hypervisor, virSecretPtr);

  class LookupByUsageWorker : public NLVLookupInstanceByValueWorker<Secret, Hypervisor, virSecretPtr> {
  public:
    LookupByUsageWorker(Nan::Callback *callback, Hypervisor *parent, const std::string &usageId, int usageType) \
      : NLVLookupInstanceByValueWorker<Secret, Hypervisor, virSecretPtr>(callback, parent, usageId), usageType_(usageType) {}
    void Execute();
  private:
    int usageType_;
  };

  // ACTION WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, virSecretPtr, bool);

  // ACCESSOR/MUTATOR WORKERS
  NLV_STRING_RETURN_WORKER(GetUUID, virSecretPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetUsageId, virSecretPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetUsageType, virSecretPtr, int);
  NLV_STRING_RETURN_WORKER(GetValue, virSecretPtr, std::string);
  NLV_STRING_RETURN_WORKER(ToXml, virSecretPtr, std::string);

  class SetValueWorker : public NLVPrimitiveReturnWorker<virSecretPtr, bool> {
  public:
    SetValueWorker(Nan::Callback *callback, virSecretPtr handle, const std::string &value)
      : NLVPrimitiveReturnWorker<virSecretPtr, bool>(callback, handle), value_(value) {} \
    void Execute();
  private:
    std::string value_;
  };

};

}  // namespace NLV

#endif  // SECRET_H
