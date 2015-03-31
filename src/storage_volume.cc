// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

#include "storage_pool.h"
#include "storage_volume.h"

namespace NodeLibvirt {

Persistent<FunctionTemplate> StorageVolume::constructor_template;
void StorageVolume::Initialize()
{
  NanScope();
  Local<FunctionTemplate> t = FunctionTemplate::New();
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "getInfo", GetInfo);
  NODE_SET_PROTOTYPE_METHOD(t, "getKey",  GetKey);
  NODE_SET_PROTOTYPE_METHOD(t, "getName", GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getPath", GetPath);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml",   ToXml);
  NODE_SET_PROTOTYPE_METHOD(t, "remove",  Delete);
  NODE_SET_PROTOTYPE_METHOD(t, "wipe",    Wipe);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(NanNew("StorageVolume"));

  Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_VOL_FILE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_VOL_BLOCK);
}

Local<Object> StorageVolume::NewInstance(const LibVirtHandle &handle)
{
  NanScope();
  StorageVolume *storageVolume = new StorageVolume(handle.ToStorageVolume());
  Local<Object> object = constructor_template->GetFunction()->NewInstance();
  storageVolume->Wrap(object);
  return NanEscapeScope(object);
}

NAN_METHOD(StorageVolume::Create)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a string and callback");
    NanReturnUndefined();
  }

  if (!NanHasInstance(StoragePool::constructor_template, args.This())) {
    NanThrowTypeError("You must specify a StoragePool instance");
    NanReturnUndefined();
  }

  StoragePool *sp = ObjectWrap::Unwrap<StoragePool>(args.This());
  std::string xmlData(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new CreateWorker(callback, sp->handle_, xmlData));
  NanReturnUndefined();
}

void StorageVolume::CreateWorker::Execute()
{
  unsigned int flags = 0;
  lookupHandle_ =
    virStorageVolCreateXML(Handle().ToStoragePool(), value_.c_str(), flags);
  if (lookupHandle_.ToStoragePool() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}


NLV_WORKER_METHOD_NO_ARGS(StorageVolume, Delete)
void StorageVolume::DeleteWorker::Execute()
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();

  unsigned int flags = 0;
  int result = virStorageVolDelete(Handle().ToStorageVolume(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  if (Handle().ToStorageVolume() != NULL) {
    Handle().Clear();
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, Wipe)
void StorageVolume::WipeWorker::Execute()
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();

  unsigned int flags = 0;
  int result = virStorageVolWipe(Handle().ToStorageVolume(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(StorageVolume::GetInfo)
{
  NanScope();
  if (args.Length() != 1) {
    NanThrowTypeError("You must specify a callback");
    NanReturnUndefined();
  }

  NanCallback *callback = new NanCallback(args[0].As<Function>());
  StorageVolume *storageVolume = ObjectWrap::Unwrap<StorageVolume>(args.This());
  NanAsyncQueueWorker(new GetInfoWorker(callback, storageVolume->handle_));
  NanReturnUndefined();
}

void StorageVolume::GetInfoWorker::Execute()
{
  int result = virStorageVolGetInfo(Handle().ToStorageVolume(), &info_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

void StorageVolume::GetInfoWorker::HandleOKCallback()
{
  NanScope();
  Local<Object> result = NanNew<Object>();
  result->Set(NanNew("type"), NanNew<Integer>(info_.type));
  result->Set(NanNew("capacity"), NanNew<Number>(info_.capacity));
  result->Set(NanNew("allocation"), NanNew<Number>(info_.allocation));

  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, GetKey)
void StorageVolume::GetKeyWorker::Execute()
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();

  const char *result = virStorageVolGetKey(Handle().ToStorageVolume());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, GetName)
void StorageVolume::GetNameWorker::Execute()
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();

  const char *result = virStorageVolGetName(Handle().ToStorageVolume());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, GetPath)
void StorageVolume::GetPathWorker::Execute()
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();

  const char *result = virStorageVolGetPath(Handle().ToStorageVolume());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(StorageVolume, ToXml)
void StorageVolume::ToXmlWorker::Execute()
{
  NLV_WORKER_ASSERT_STORAGEVOLUME();

  unsigned int flags = 0;
  char *result = virStorageVolGetXMLDesc(Handle().ToStorageVolume(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_SP_LOOKUP_BY_VALUE_EXECUTE(StorageVolume, LookupByName, virStorageVolLookupByName)
NAN_METHOD(StorageVolume::LookupByName)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid name and callback.");
    NanReturnUndefined();
  }

  Local<Object> object = args.This();
  if (!NanHasInstance(StoragePool::constructor_template, object)) {
    NanThrowTypeError("You must specify a StoragePool instance");
    NanReturnUndefined();
  }

  StoragePool *sp = ObjectWrap::Unwrap<StoragePool>(object);
  std::string name(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByNameWorker(callback, sp->handle_, name));
  NanReturnUndefined();
}

NLV_LOOKUP_BY_VALUE_EXECUTE(StorageVolume, LookupByKey, virStorageVolLookupByKey)
NAN_METHOD(StorageVolume::LookupByKey)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid name and callback.");
    NanReturnUndefined();
  }

  Local<Object> object = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, object)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(object);
  std::string key(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByKeyWorker(callback, hv->handle_, key));
  NanReturnUndefined();
}

NLV_LOOKUP_BY_VALUE_EXECUTE(StorageVolume, LookupByPath, virStorageVolLookupByPath)
NAN_METHOD(StorageVolume::LookupByPath)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid name and callback.");
    NanReturnUndefined();
  }

  Local<Object> object = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, object)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(object);
  std::string path(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByPathWorker(callback, hv->handle_, path));
  NanReturnUndefined();
}

NAN_METHOD(StorageVolume::Clone)
{
  NanScope();
  if (args.Length() < 3) {
    NanThrowTypeError("You must specify three arguments to call this function");
    NanReturnUndefined();
  }

  if (!NanHasInstance(StorageVolume::constructor_template, args[0])) {
    NanThrowTypeError("You must specify a StorageVolume instance as first argument");
    NanReturnUndefined();
  }

  if (!args[1]->IsString()) {
    NanThrowTypeError("You must specify a string as second argument");
    NanReturnUndefined();
  }

  if (!args[2]->IsFunction()) {
    NanThrowTypeError("You must specify a callback as the third argument");
    NanReturnUndefined();
  }

  Local<Object> object = args.This();
  if (!NanHasInstance(StoragePool::constructor_template, object)) {
    NanThrowTypeError("You must specify a StoragePool instance");
    NanReturnUndefined();
  }

  std::string xml(*NanUtf8String(args[1]->ToString()));
  StoragePool *sp = ObjectWrap::Unwrap<StoragePool>(object);
  StorageVolume *sv = ObjectWrap::Unwrap<StorageVolume>(args[0]->ToObject());

  NanCallback *callback = new NanCallback(args[2].As<Function>());
  NanAsyncQueueWorker(new CloneWorker(callback, sp->handle_, xml, sv->handle_));
  NanReturnUndefined();
}

void StorageVolume::CloneWorker::Execute()
{
  unsigned int flags = 0;
  lookupHandle_ =
    virStorageVolCreateXMLFrom(Handle().ToStoragePool(), value_.c_str(), cloneHandle_, flags);
  if (lookupHandle_.ToStorageVolume() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

// NAN_METHOD(StorageVolume::Clone)
// {
//   NanScope();

//   unsigned int flags = 0;
//   if (args.Length() < 2) {
//     NanThrowTypeError("You must specify two arguments to call this function");
//     NanReturnUndefined();
//   }

//   if (!NanHasInstance(StorageVolume::constructor_template, args[0])) {
//     NanThrowTypeError("You must specify a StorageVolume instance as first argument");
//     NanReturnUndefined();
//   }

//   if (!args[1]->IsString()) {
//     NanThrowTypeError("You must specify a string as second argument");
//     NanReturnUndefined();
//   }

//   Local<Object> pool_obj = args.This();
//   if (!NanHasInstance(StoragePool::constructor_template, pool_obj)) {
//     NanThrowTypeError("You must specify a StoragePool instance");
//     NanReturnUndefined();
//   }

//   String::Utf8Value xml(args[1]->ToString());
//   StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(pool_obj);
//   StorageVolume *source_volume = ObjectWrap::Unwrap<StorageVolume>(args[0]->ToObject());

//   virStorageVolPtr handle =
//     virStorageVolCreateXMLFrom(pool->Pool(), (const char *) *xml, source_volume->handle_, flags);
//   if (handle == NULL) {
//     ThrowException(Error::New(virGetLastError()));
//     NanReturnUndefined();
//   }

//   StorageVolume *clone_volume = new StorageVolume(handle);

//   Local<Object> clone_vol_obj = constructor_template->GetFunction()->NewInstance();
//   clone_volume->Wrap(clone_vol_obj);
//   NanReturnValue(clone_vol_obj);
// }

} //namespace NodeLibvirt

