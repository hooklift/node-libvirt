// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_STORAGE_POOL_H_
#define SRC_STORAGE_POOL_H_

#include "node_libvirt.h"

#include "nlv_async_worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class StoragePool : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virStoragePoolPtr handle);
  virtual ~StoragePool();

private:
  explicit StoragePool(virStoragePoolPtr handle) : handle_(handle) {}
  static Persistent<FunctionTemplate> constructor_template;
  static Persistent<Function> constructor;
  virStoragePoolPtr handle_;

  friend class StorageVolume;
  friend class Hypervisor;

private:
  // HYPERVISOR METHODS
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByUUID);
  static NAN_METHOD(LookupByVolume);
  static NAN_METHOD(Define);
  static NAN_METHOD(Create);

  // ACTIONS
  static NAN_METHOD(Build);
  static NAN_METHOD(Undefine);
  static NAN_METHOD(Start);
  static NAN_METHOD(Stop);
  static NAN_METHOD(Erase);
  static NAN_METHOD(Refresh);

  // ACCESSORS/MUTATORS
  static NAN_METHOD(GetAutostart);
  static NAN_METHOD(SetAutostart);
  static NAN_METHOD(GetInfo);
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetUUID);
  static NAN_METHOD(ToXml);
  static NAN_METHOD(IsActive);
  static NAN_METHOD(IsPersistent);
  static NAN_METHOD(GetVolumes);

private:
  // HYPERVISOR METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByName, StoragePool, virConnectPtr, virStoragePoolPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByUUID, StoragePool, virConnectPtr, virStoragePoolPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Define, StoragePool, virConnectPtr, virStoragePoolPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Create, StoragePool, virConnectPtr, virStoragePoolPtr);

  // METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Build, virStoragePoolPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, virStoragePoolPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Start, virStoragePoolPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Stop, virStoragePoolPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Refresh, virStoragePoolPtr, bool);

  class EraseWorker : public NLVPrimitiveReturnWorker<virStoragePoolPtr, bool> {
  public:
    EraseWorker(NanCallback *callback, virStoragePoolPtr handle, unsigned int flags)
      : NLVPrimitiveReturnWorker<virStoragePoolPtr, bool>(callback, handle), flags_(flags) {}
    void Execute();
  private:
    unsigned int flags_;
  };

  // ACCESSORS/MUTATORS WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(GetAutostart, virStoragePoolPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetName, virStoragePoolPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetUUID, virStoragePoolPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, virStoragePoolPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(IsActive, virStoragePoolPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsPersistent, virStoragePoolPtr, bool);
  NLV_OBJECT_RETURN_WORKER(GetInfo, virStoragePoolPtr, virStoragePoolInfo);
  NLV_LIST_RETURN_WORKER(GetVolumes, virStoragePoolPtr, std::string, v8::String);

  class SetAutostartWorker : public NLVPrimitiveReturnWorker<virStoragePoolPtr, bool> {
  public:
    SetAutostartWorker(NanCallback *callback, virStoragePoolPtr handle, bool autoStart)
      : NLVPrimitiveReturnWorker<virStoragePoolPtr, bool>(callback, handle), autoStart_(autoStart) {}
    void Execute();
  private:
    bool autoStart_;
  };

};

}  //namespace NodeLibvirt

#endif  // SRC_STORAGE_POOL_H
