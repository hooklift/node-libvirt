// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

#include "storage_pool.h"
#include "storage_volume.h"
#include "worker.h"

namespace NLV {

Nan::Persistent<FunctionTemplate> StorageVolume::constructor_template;
Nan::Persistent<Function> StorageVolume::constructor;
void StorageVolume::Initialize(Handle<Object> exports)
{
  Nan::HandleScope scope;
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>();
  t->SetClassName(Nan::New("StorageVolume").ToLocalChecked());
  t->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(t, "getInfo", GetInfo);
  Nan::SetPrototypeMethod(t, "getKey",  GetKey);
  Nan::SetPrototypeMethod(t, "getName", GetName);
  Nan::SetPrototypeMethod(t, "getPath", GetPath);
  Nan::SetPrototypeMethod(t, "toXml",   ToXml);
  Nan::SetPrototypeMethod(t, "remove",  Delete);
  Nan::SetPrototypeMethod(t, "wipe",    Wipe);

  constructor_template.Reset(t);
  constructor.Reset(t->GetFunction());
  exports->Set(Nan::New("StorageVolume").ToLocalChecked(), t->GetFunction());

  // Constants
  NODE_DEFINE_CONSTANT(exports, VIR_STORAGE_VOL_FILE);
  NODE_DEFINE_CONSTANT(exports, VIR_STORAGE_VOL_BLOCK);
}

StorageVolume::StorageVolume(virStorageVolPtr handle) : NLVObject(handle) {}

NAN_METHOD(StorageVolume::Create)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a string and callback");
    return;
  }
  auto sp = StoragePool::UnwrapHandle(info.This());
  std::string xmlData(*Nan::Utf8String(info[0]->ToString()));
  unsigned int flags = GetFlags(info[1]);

  Worker::RunAsync(info, [=](Worker::SetOnFinishedHandler onFinished) {
    auto handle = virStorageVolCreateXML(sp, xmlData.c_str(), flags);
    if (handle == NULL) {
      return virSaveLastError();
    }
    return onFinished(InstanceReturnHandler<StorageVolume>(handle));
  });
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, Delete)
NLV_WORKER_EXECUTE(StorageVolume, Delete)
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();
  unsigned int flags = 0;
  int result = virStorageVolDelete(Handle(), flags);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, Wipe)
NLV_WORKER_EXECUTE(StorageVolume, Wipe)
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();
  unsigned int flags = 0;
  int result = virStorageVolWipe(Handle(), flags);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(StorageVolume::GetInfo)
{
  Nan::HandleScope scope;
  if (info.Length() != 1) {
    Nan::ThrowTypeError("You must specify a callback");
    return;
  }

  Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());
  StorageVolume *storageVolume = StorageVolume::Unwrap(info.This());
  Nan::AsyncQueueWorker(new GetInfoWorker(callback, storageVolume->virHandle()));
  return;
}

NLV_WORKER_EXECUTE(StorageVolume, GetInfo)
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();
  int result = virStorageVolGetInfo(Handle(), &info_);
  if (result == -1) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(StorageVolume, GetInfo)
{
  Nan::HandleScope scope;
  Local<Object> result = Nan::New<Object>();
  result->Set(Nan::New("type").ToLocalChecked(), Nan::New<Integer>(info_.type));
  result->Set(Nan::New("capacity").ToLocalChecked(), Nan::New<Number>(info_.capacity));
  result->Set(Nan::New("allocation").ToLocalChecked(), Nan::New<Number>(info_.allocation));

  v8::Local<v8::Value> argv[] = { Nan::Null(), result };
  callback->Call(2, argv);
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, GetKey)
NLV_WORKER_EXECUTE(StorageVolume, GetKey)
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();
  const char *result = virStorageVolGetKey(Handle());
  if (result == NULL) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, GetName)
NLV_WORKER_EXECUTE(StorageVolume, GetName)
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();
  const char *result = virStorageVolGetName(Handle());
  if (result == NULL) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, GetPath)
NLV_WORKER_EXECUTE(StorageVolume, GetPath)
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();
  const char *result = virStorageVolGetPath(Handle());
  if (result == NULL) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, ToXml)
NLV_WORKER_EXECUTE(StorageVolume, ToXml)
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();
  unsigned int flags = 0;
  char *result = virStorageVolGetXMLDesc(Handle(), flags);
  if (result == NULL) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(StorageVolume, LookupByName, virStorageVolLookupByName)
NAN_METHOD(StorageVolume::LookupByName)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a valid name and callback.");
    return;
  }

  Local<Object> object = info.This();
  if (!Nan::New(StoragePool::constructor_template)->HasInstance(object)) {
    Nan::ThrowTypeError("You must specify a StoragePool instance");
    return;
  }

  StoragePool *sp = StoragePool::Unwrap(object);
  std::string name(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  NLV::AsyncQueueWorker(new LookupByNameWorker(callback, sp, name), info.This());
  return;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(StorageVolume, LookupByKey, virStorageVolLookupByKey)
NAN_METHOD(StorageVolume::LookupByKey)
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

  Hypervisor *hv = Hypervisor::Unwrap(object);
  std::string key(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  NLV::AsyncQueueWorker(new LookupByKeyWorker(callback, hv, key), info.This());
  return;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(StorageVolume, LookupByPath, virStorageVolLookupByPath)
NAN_METHOD(StorageVolume::LookupByPath)
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

  Hypervisor *hv = Hypervisor::Unwrap(object);
  std::string path(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  NLV::AsyncQueueWorker(new LookupByPathWorker(callback, hv, path), info.This());
  return;
}

NAN_METHOD(StorageVolume::Clone)
{
  Nan::HandleScope scope;
  if (info.Length() < 3) {
    Nan::ThrowTypeError("You must specify three arguments to call this function");
    return;
  }

  if (!Nan::New(StorageVolume::constructor_template)->HasInstance(info[0])) {
    Nan::ThrowTypeError("You must specify a StorageVolume instance as first argument");
    return;
  }

  if (!info[1]->IsString()) {
    Nan::ThrowTypeError("You must specify a string as second argument");
    return;
  }

  if (!info[2]->IsFunction()) {
    Nan::ThrowTypeError("You must specify a callback as t.ToLocalChecked()he third argument");
    return;
  }

  Local<Object> object = info.This();
  if (!Nan::New(StoragePool::constructor_template)->HasInstance(object)) {
    Nan::ThrowTypeError("You must specify a StoragePool instance");
    return;
  }

  std::string xml(*Nan::Utf8String(info[1]->ToString()));
  StoragePool *sp = StoragePool::Unwrap(object);
  StorageVolume *sv = StorageVolume::Unwrap(info[0]->ToObject());
  Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());
  NLV::AsyncQueueWorker(new CloneWorker(callback, sp, xml, sv->virHandle()), info.This());
  return;
}

NLV_WORKER_EXECUTE(StorageVolume, Clone)
{
  NLV_WORKER_ASSERT_PARENT_HANDLE();
  unsigned int flags = 0;
  lookupHandle_ =
    virStorageVolCreateXMLFrom(parent_->virHandle(), value_.c_str(), cloneHandle_, flags);
  if (lookupHandle_ == NULL) {
    SET_ERROR_WITH_CONTEXT(virSaveLastError());
    return;
  }
}

} //namespace NLV
