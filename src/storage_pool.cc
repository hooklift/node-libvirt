// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

#include "storage_volume.h"
#include "storage_pool.h"
#include "worker.h"
//FIXME default params, default flags

namespace NLV {

Nan::Persistent<FunctionTemplate> StoragePool::constructor_template;
Nan::Persistent<Function> StoragePool::constructor;
void StoragePool::Initialize(Handle<Object> exports)
{
  Nan::HandleScope scope;
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>();
  t->SetClassName(Nan::New("StoragePool").ToLocalChecked());
  t->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(t, "build",               Build);
  Nan::SetPrototypeMethod(t, "start",               Start);
  Nan::SetPrototypeMethod(t, "stop",                Stop);
  Nan::SetPrototypeMethod(t, "erase",               Erase);
  Nan::SetPrototypeMethod(t, "setAutostart",        SetAutostart);
  Nan::SetPrototypeMethod(t, "getAutostart",        GetAutostart);
  Nan::SetPrototypeMethod(t, "getInfo",             GetInfo);
  Nan::SetPrototypeMethod(t, "getName",             GetName);
  Nan::SetPrototypeMethod(t, "getUUID",             GetUUID);
  Nan::SetPrototypeMethod(t, "undefine",            Undefine);
  Nan::SetPrototypeMethod(t, "toXml",               ToXml);
  Nan::SetPrototypeMethod(t, "isActive",            IsActive);
  Nan::SetPrototypeMethod(t, "isPersistent",        IsPersistent);
  Nan::SetPrototypeMethod(t, "getVolumes",          GetVolumes);
  Nan::SetPrototypeMethod(t, "refresh",             Refresh);
  Nan::SetPrototypeMethod(t, "createVolume",        StorageVolume::Create);
  Nan::SetPrototypeMethod(t, "cloneVolume",         StorageVolume::Clone);
  Nan::SetPrototypeMethod(t, "lookupStorageVolumeByName",  StorageVolume::LookupByName);

  constructor_template.Reset(t);
  constructor.Reset(t->GetFunction());
  exports->Set(Nan::New("StoragePool").ToLocalChecked(), t->GetFunction());

  //Constants initialization
  //virStoragePoolDeleteFlags
  NODE_DEFINE_CONSTANT(exports, VIR_STORAGE_POOL_DELETE_NORMAL);
  NODE_DEFINE_CONSTANT(exports, VIR_STORAGE_POOL_DELETE_ZEROED);

  //virStoragePoolState
  NODE_DEFINE_CONSTANT(exports, VIR_STORAGE_POOL_INACTIVE);
  NODE_DEFINE_CONSTANT(exports, VIR_STORAGE_POOL_BUILDING);
  NODE_DEFINE_CONSTANT(exports, VIR_STORAGE_POOL_RUNNING);
  NODE_DEFINE_CONSTANT(exports, VIR_STORAGE_POOL_DEGRADED);
  NODE_DEFINE_CONSTANT(exports, VIR_STORAGE_POOL_INACCESSIBLE);
}

StoragePool::StoragePool(virStoragePoolPtr handle) : NLVObject(handle) {}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(StoragePool, LookupByName, virStoragePoolLookupByName)
NAN_METHOD(StoragePool::LookupByName)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a valid name and callback.");
    return;
  }

  Local<Object> object = info.This();
  if (!Nan::New(Hypervisor::constructor_template)->HasInstance(object)) {
    Nan::ThrowTypeError("You must specify a Hypervisor instance");
    return;
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(object);
  std::string name(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  NLV::AsyncQueueWorker(new LookupByNameWorker(callback, hv, name), info.This());
  return;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(StoragePool, LookupByUUID, virStoragePoolLookupByUUIDString)
NAN_METHOD(StoragePool::LookupByUUID)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a valid uuid and callback.");
    return;
  }

  if (!Nan::New(Hypervisor::constructor_template)->HasInstance(info.This())) {
    Nan::ThrowTypeError("You must specify a Hypervisor instance");
    return;
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(info.This());
  std::string uuid(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  NLV::AsyncQueueWorker(new LookupByUUIDWorker(callback, hv, uuid), info.This());
  return;
}

NAN_METHOD(StoragePool::LookupByVolume)
{
  Nan::HandleScope scope;
  auto volume = StorageVolume::UnwrapHandle(info[0]);

  Worker::RunAsync(info, [=](Worker::SetOnFinishedHandler onFinished) {
    auto storagePool = virStoragePoolLookupByVolume(volume);
    if (!storagePool) {
      return virSaveLastError();
    }

    return onFinished(InstanceReturnHandler<StoragePool>(storagePool));
  });
}

NAN_METHOD(StoragePool::Create)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a string and callback");
    return;
  }

  auto hv = Hypervisor::UnwrapHandle(info.This());
  std::string xmlData(*Nan::Utf8String(info[0]->ToString()));

  Worker::RunAsync(info, [=](Worker::SetOnFinishedHandler onFinished) {
    auto lookupHandle = virStoragePoolCreateXML(hv, xmlData.c_str(), 0);
    if (lookupHandle == NULL) {
      return virSaveLastError();
    }
    return onFinished(InstanceReturnHandler<StoragePool>(lookupHandle));
  });
}

NLV_WORKER_EXECUTE(StoragePool, Create)
{
  NLV_WORKER_ASSERT_PARENT_HANDLE();
  unsigned int flags = 0;
  lookupHandle_ = virStoragePoolCreateXML(parent_->virHandle(), value_.c_str(), flags);
  if (lookupHandle_ == NULL) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }
}

NLV_WORKER_METHOD_DEFINE(StoragePool)
NLV_WORKER_EXECUTE(StoragePool, Define)
{
  NLV_WORKER_ASSERT_PARENT_HANDLE();
  unsigned int flags = 0;
  lookupHandle_ = virStoragePoolDefineXML(parent_->virHandle(), value_.c_str(), flags);
  if (lookupHandle_ == NULL) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, Build)
NLV_WORKER_EXECUTE(StoragePool, Build)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  unsigned int flags = 0;
  int result = virStoragePoolBuild(Handle(), flags);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, Undefine)
NLV_WORKER_EXECUTE(StoragePool, Undefine)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  int result = virStoragePoolUndefine(Handle());
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, Start)
NLV_WORKER_EXECUTE(StoragePool, Start)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  unsigned int flags = 0;
  int result = virStoragePoolCreate(Handle(), flags);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, Stop)
NLV_WORKER_EXECUTE(StoragePool, Stop)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  int result = virStoragePoolDestroy(Handle());
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(StoragePool::Erase)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsArray() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify an array of flags and a callback");
    return;
  }

  unsigned int flags = 0;
  Local<Array> flagsArray = Local<Array>::Cast(info[0]);
  unsigned int length = flagsArray->Length();
  for (unsigned int i = 0; i < length; ++i) {
    flags |= flagsArray->Get(Nan::New(i))->Int32Value();
  }

  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  StoragePool *storagePool = ObjectWrap::Unwrap<StoragePool>(info.This());
  Nan::AsyncQueueWorker(new EraseWorker(callback, storagePool->virHandle(), flags));
  return;
}

NLV_WORKER_EXECUTE(StoragePool, Erase)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  if (flags_ == 0) {
    flags_ |= VIR_STORAGE_POOL_DELETE_NORMAL;
  }

  int result = virStoragePoolDelete(Handle(), flags_);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, GetAutostart)
NLV_WORKER_EXECUTE(StoragePool, GetAutostart)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  int autostart;
  int result = virStoragePoolGetAutostart(Handle(), &autostart);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = static_cast<bool>(autostart);
}

NAN_METHOD(StoragePool::SetAutostart)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a bool and callback");
    return;
  }

  bool autoStart = info[0]->IsTrue();
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  StoragePool *storagePool = ObjectWrap::Unwrap<StoragePool>(info.This());
  Nan::AsyncQueueWorker(new SetAutostartWorker(callback, storagePool->virHandle(), autoStart));
  return;
}

NLV_WORKER_EXECUTE(StoragePool, SetAutostart)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  int result = virStoragePoolSetAutostart(Handle(), autoStart_ ? 1 : 0);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, GetInfo)
NLV_WORKER_EXECUTE(StoragePool, GetInfo)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  int result = virStoragePoolGetInfo(Handle(), &info_);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(StoragePool, GetInfo)
{
  Nan::HandleScope scope;
  Local<Object> result = Nan::New<Object>();
  result->Set(Nan::New("state").ToLocalChecked(), Nan::New<Integer>(info_.state));
  result->Set(Nan::New("capacity").ToLocalChecked(), Nan::New<Number>(info_.capacity));
  result->Set(Nan::New("allocation").ToLocalChecked(), Nan::New<Number>(info_.allocation));
  result->Set(Nan::New("available").ToLocalChecked(), Nan::New<Number>(info_.available));

  v8::Local<v8::Value> argv[] = { Nan::Null(), result };
  callback->Call(2, argv);
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, GetName)
NLV_WORKER_EXECUTE(StoragePool, GetName)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  const char *result = virStoragePoolGetName(Handle());
  if (result == NULL) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, GetUUID)
NLV_WORKER_EXECUTE(StoragePool, GetUUID)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  int result = virStoragePoolGetUUIDString(Handle(), uuid);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    delete[] uuid;
    return;
  }

  data_ = uuid;
  delete[] uuid;
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, ToXml)
NLV_WORKER_EXECUTE(StoragePool, ToXml)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  unsigned int flags = 0;
  char *result = virStoragePoolGetXMLDesc(Handle(), flags);
  if (result == NULL) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, IsActive)
NLV_WORKER_EXECUTE(StoragePool, IsActive)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  int result = virStoragePoolIsActive(Handle());
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, IsPersistent)
NLV_WORKER_EXECUTE(StoragePool, IsPersistent)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  int result = virStoragePoolIsPersistent(Handle());
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NAN_METHOD(StoragePool::GetVolumes)
{
  Nan::HandleScope scope;
  if (info.Length() != 1) {
    Nan::ThrowTypeError("You must specify a callback");
    return;
  }

  Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());
  StoragePool *storagePool = ObjectWrap::Unwrap<StoragePool>(info.This());
  Nan::AsyncQueueWorker(new GetVolumesWorker(callback, storagePool->virHandle()));
  return;
}

NLV_WORKER_EXECUTE(StoragePool, GetVolumes)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  int num_volumes = virStoragePoolNumOfVolumes(Handle());
  if (num_volumes == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  char **volumes = (char**) malloc(sizeof(*volumes) * num_volumes);
  if (volumes == NULL) {
    SetErrorMessage("unable to allocate memory");
    return;
  }

  num_volumes = virStoragePoolListVolumes(Handle(), volumes, num_volumes);
  if (num_volumes == -1) {
    free(volumes);
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  for (int i = 0; i < num_volumes; ++i) {
    // TODO new StorageVolume and return array of StorageVolume instances
    data_.push_back(volumes[i]);
    free(volumes[i]);
  }

  free(volumes);
}

NLV_WORKER_METHOD_NO_ARGS(StoragePool, Refresh)
NLV_WORKER_EXECUTE(StoragePool, Refresh)
{
  NLV_WORKER_ASSERT_STORAGEPOOL();
  unsigned int flags = 0;
  int result = virStoragePoolRefresh(Handle(), flags);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = true;
}

} //namespace NLV
