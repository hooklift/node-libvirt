// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_STORAGE_VOLUME_H_
#define SRC_STORAGE_VOLUME_H_

#include "nlv_object.h"
#include "nlv_async_worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

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
  static Persistent<FunctionTemplate> constructor_template;
  static Persistent<Function> constructor;
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
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByKey, StorageVolume, virConnectPtr, virStorageVolPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByPath, StorageVolume, virConnectPtr, virStorageVolPtr);

  // STORAGEPOOL METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByName, StorageVolume, virStoragePoolPtr, virStorageVolPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Create, StorageVolume, virStoragePoolPtr, virStorageVolPtr);

  class CloneWorker : public NLVLookupInstanceByValueWorker<StorageVolume, virStoragePoolPtr, virStorageVolPtr> {
  public:
    CloneWorker(NanCallback *callback, virStoragePoolPtr handle, const std::string &value, virStorageVolPtr cloneHandle)
      : NLVLookupInstanceByValueWorker<StorageVolume, virStoragePoolPtr, virStorageVolPtr>(callback, handle, value), cloneHandle_(cloneHandle) {}
    void Execute();
  private:
    virStorageVolPtr cloneHandle_;
  };

  // ACTION METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Wipe, virStorageVolPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Delete, virStorageVolPtr, bool);

  // ACCESSOR/MUTATOR WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(GetKey, virStorageVolPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetName, virStorageVolPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetPath, virStorageVolPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, virStorageVolPtr, std::string);
  NLV_OBJECT_RETURN_WORKER(GetInfo, virStorageVolPtr, virStorageVolInfo);

};

}  //namespace NodeLibvirt

#endif  // SRC_STORAGE_VOLUME_H
