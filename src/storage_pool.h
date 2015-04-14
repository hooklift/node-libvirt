// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_STORAGE_POOL_H_
#define SRC_STORAGE_POOL_H_

#include "node_libvirt.h"

#include "worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class StoragePool : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(const LibVirtHandle &handle);
  virtual ~StoragePool();

private:
  explicit StoragePool(virStoragePoolPtr handle) : handle_(handle) {}
  static Persistent<FunctionTemplate> constructor_template;
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
  NLV_LOOKUP_BY_VALUE_WORKER(StoragePool, LookupByName);
  NLV_LOOKUP_BY_VALUE_WORKER(StoragePool, LookupByUUID);
  NLV_LOOKUP_BY_VALUE_WORKER(StoragePool, Define);
  NLV_LOOKUP_BY_VALUE_WORKER(StoragePool, Create);

  // METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Build, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Start, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Stop, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Refresh, bool);

  class EraseWorker : public PrimitiveReturnWorker<bool> {
  public:
    EraseWorker(NanCallback *callback, const LibVirtHandle &handle, unsigned int flags)
      : PrimitiveReturnWorker<bool>(callback, handle), flags_(flags) {}
    void Execute();
  private:
    unsigned int flags_;
  };

  // ACCESSORS/MUTATORS WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(GetAutostart, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetName, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetUUID, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(IsActive, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsPersistent, bool);
  NLV_OBJECT_RETURN_WORKER(GetInfo, virStoragePoolInfo);
  NLV_LIST_RETURN_WORKER(GetVolumes, std::string, v8::String);

  class SetAutostartWorker : public PrimitiveReturnWorker<bool> {
  public:
    SetAutostartWorker(NanCallback *callback, const LibVirtHandle &handle, bool autoStart)
      : PrimitiveReturnWorker<bool>(callback, handle), autoStart_(autoStart) {}
    void Execute();
  private:
    bool autoStart_;
  };

};

}  //namespace NodeLibvirt

#endif  // SRC_STORAGE_POOL_H
