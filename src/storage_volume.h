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
  static Local<Object> NewInstance(const LibVirtHandle &handle);
  virtual ~StorageVolume();

private:
  explicit StorageVolume(virStorageVolPtr handle) : handle_(handle) {}
  static Persistent<FunctionTemplate> constructor_template;
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
  NLV_LOOKUP_BY_VALUE_WORKER(StorageVolume, LookupByKey);
  NLV_LOOKUP_BY_VALUE_WORKER(StorageVolume, LookupByPath);

  // STORAGEPOOL METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(StorageVolume, LookupByName);
  NLV_LOOKUP_BY_VALUE_WORKER(StorageVolume, Create);

  class CloneWorker : public LookupInstanceByValueWorker<StorageVolume> {
  public:
    CloneWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &value, virStorageVolPtr cloneHandle)
      : LookupInstanceByValueWorker<StorageVolume>(callback, handle, value), cloneHandle_(cloneHandle) {}
    void Execute();
  private:
    virStorageVolPtr cloneHandle_;
  };

  // ACTION METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Wipe, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Delete, bool);

  // ACCESSOR/MUTATOR WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(GetKey, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetName, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetPath, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, std::string);
  NLV_OBJECT_RETURN_WORKER(GetInfo, virStorageVolInfo);

};

}  //namespace NodeLibvirt

#endif  // SRC_STORAGE_VOLUME_H
