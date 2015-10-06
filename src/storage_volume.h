// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef STORAGE_VOLUME_H
#define STORAGE_VOLUME_H

#include "nlv_object.h"
#include "nlv_async_worker.h"
#include "worker_macros.h"

#include "hypervisor.h"
#include "storage_pool.h"

namespace NLV {

struct StorageVolumeCleanupHandler {
  static int cleanup(virStorageVolPtr handle) {
    return virStorageVolFree(handle);
  }
};

class StorageVolume : public NLVObject<virStorageVolPtr, StorageVolumeCleanupHandler>
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virStorageVolPtr handle);

private:
  explicit StorageVolume(virStorageVolPtr handle);
  static Nan::Persistent<FunctionTemplate> constructor_template;
  static Nan::Persistent<Function> constructor;
  friend class StoragePool;
  friend class Hypervisor;

private:
  // HYPERVISOR METHODS
  static NAN_METHOD(LookupByKey);
  static NAN_METHOD(LookupByPath);

  // STORAGEPOOL METHODS
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(Create);
  static NAN_METHOD(Clone);

  // ACTIONS
  static NAN_METHOD(Wipe);
  static NAN_METHOD(Delete);

  // ACCESSORS/MUTATORS
  static NAN_METHOD(GetInfo);
  static NAN_METHOD(GetKey);
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetPath);
  static NAN_METHOD(ToXml);

private:
  // HYPERVISOR METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByKey, StorageVolume, Hypervisor, virStorageVolPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByPath, StorageVolume, Hypervisor, virStorageVolPtr);

  // STORAGEPOOL METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByName, StorageVolume, StoragePool, virStorageVolPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Create, StorageVolume, StoragePool, virStorageVolPtr);

  class CloneWorker : public NLVLookupInstanceByValueWorker<StorageVolume, StoragePool, virStorageVolPtr> {
  public:
    CloneWorker(Nan::Callback *callback, StoragePool *parent, const std::string &value, virStorageVolPtr cloneHandle)
      : NLVLookupInstanceByValueWorker<StorageVolume, StoragePool, virStorageVolPtr>(callback, parent, value), cloneHandle_(cloneHandle) {}
    void Execute();
  private:
    virStorageVolPtr cloneHandle_;
  };

  // ACTION METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Wipe, virStorageVolPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Delete, virStorageVolPtr, bool);

  // ACCESSOR/MUTATOR WORKERS
  NLV_STRING_RETURN_WORKER(GetKey, virStorageVolPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetName, virStorageVolPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetPath, virStorageVolPtr, std::string);
  NLV_STRING_RETURN_WORKER(ToXml, virStorageVolPtr, std::string);
  NLV_OBJECT_RETURN_WORKER(GetInfo, virStorageVolPtr, virStorageVolInfo);

};

}  //namespace NLV

#endif  // STORAGE_VOLUME_H
