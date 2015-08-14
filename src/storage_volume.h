// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_STORAGE_VOLUME_H_
#define SRC_STORAGE_VOLUME_H_

#include "node_libvirt.h"

#include "worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class StorageVolume : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance2(virStorageVolPtr handle);
  virtual ~StorageVolume();

private:
  explicit StorageVolume(virStorageVolPtr handle) : handle_(handle) {}
  static Persistent<FunctionTemplate> constructor_template;
  static Persistent<Function> constructor;
  virStorageVolPtr handle_;

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
  NLV_LOOKUP_BY_VALUE_WORKER2(LookupByKey, StorageVolume, virConnectPtr, virStorageVolPtr);
  NLV_LOOKUP_BY_VALUE_WORKER2(LookupByPath, StorageVolume, virConnectPtr, virStorageVolPtr);

  // STORAGEPOOL METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER2(LookupByName, StorageVolume, virStoragePoolPtr, virStorageVolPtr);
  NLV_LOOKUP_BY_VALUE_WORKER2(Create, StorageVolume, virStoragePoolPtr, virStorageVolPtr);

  class CloneWorker : public NLVLookupInstanceByValueWorker<StorageVolume, virStoragePoolPtr, virStorageVolPtr> {
  public:
    CloneWorker(NanCallback *callback, virStoragePoolPtr handle, const std::string &value, virStorageVolPtr cloneHandle)
      : NLVLookupInstanceByValueWorker<StorageVolume, virStoragePoolPtr, virStorageVolPtr>(callback, handle, value), cloneHandle_(cloneHandle) {}
    void Execute();
  private:
    virStorageVolPtr cloneHandle_;
  };

  // ACTION METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER2(Wipe, virStorageVolPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER2(Delete, virStorageVolPtr, bool);

  // ACCESSOR/MUTATOR WORKERS
  NLV_PRIMITIVE_RETURN_WORKER2(GetKey, virStorageVolPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetName, virStorageVolPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetPath, virStorageVolPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(ToXml, virStorageVolPtr, std::string);
  NLV_OBJECT_RETURN_WORKER2(GetInfo, virStorageVolPtr, virStorageVolInfo);

};

}  //namespace NodeLibvirt

#endif  // SRC_STORAGE_VOLUME_H
