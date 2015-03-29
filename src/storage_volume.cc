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
  constructor_template->SetClassName(String::NewSymbol("StorageVolume"));

  Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_VOL_FILE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_VOL_BLOCK);
}

NAN_METHOD(StorageVolume::Create)
{
  NanScope();

  unsigned int flags = 0;
  int argsl = args.Length();
  if (argsl == 0) {
    NanThrowTypeError("You must specify at least one argument");
    NanReturnUndefined();
  }

  if (!args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as first argument");
    NanReturnUndefined();
  }

  Local<Object> pool_obj = args.This();
  if (!NanHasInstance(StoragePool::constructor_template, pool_obj)) {
    NanThrowTypeError("You must specify a Hypervisor object instance");
    NanReturnUndefined();
  }

  String::Utf8Value xml(args[0]->ToString());
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(pool_obj);
  StorageVolume *volume = new StorageVolume();
  volume->volume_ = virStorageVolCreateXML(pool->Pool(), (const char *) *xml, flags);
  if (volume->volume_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> volume_obj = constructor_template->GetFunction()->NewInstance();
  volume->Wrap(volume_obj);
  NanReturnValue(volume_obj);
}

NAN_METHOD(StorageVolume::Delete)
{
  NanScope();

  unsigned int flags = 0;
  int ret = -1;
  StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());
  ret = virStorageVolDelete(volume->volume_, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  if (volume->volume_ != NULL) {
    virStorageVolFree(volume->volume_);
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(StorageVolume::Wipe)
{
  NanScope();

  unsigned int flags = 0;
  int ret = -1;
  StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());
  ret = virStorageVolWipe(volume->volume_, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(StorageVolume::GetInfo)
{
  NanScope();

  virStorageVolInfo info;
  int ret = -1;
  StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());
  ret = virStorageVolGetInfo(volume->volume_, &info);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> object = NanNew<Object>();
  object->Set(NanNew("type"), NanNew<Integer>(info.type)); //virStoragePoolState
  object->Set(NanNew("capacity"), NanNew<Number>(info.capacity)); //bytes
  object->Set(NanNew("allocation"), NanNew<Number>(info.allocation)); //bytes
  NanReturnValue(object);
}

NAN_METHOD(StorageVolume::GetKey)
{
  NanScope();

  const char *key = NULL;
  StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());
  key = virStorageVolGetKey(volume->volume_);
  if (key == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(key));
}

NAN_METHOD(StorageVolume::GetName)
{
  NanScope();

  const char *name = NULL;
  StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());
  name = virStorageVolGetName(volume->volume_);
  if (name == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(name));
}

NAN_METHOD(StorageVolume::GetPath)
{
  NanScope();

  const char *path = NULL;
  StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());
  path = virStorageVolGetPath(volume->volume_);
  if (path == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(path));
}

NAN_METHOD(StorageVolume::ToXml)
{
  NanScope();

  char* xml_ = NULL;
  unsigned int flags = 0;
  StorageVolume *volume = ObjectWrap::Unwrap<StorageVolume>(args.This());
  xml_ = virStorageVolGetXMLDesc(volume->volume_, flags);
  if (xml_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<String> xml = NanNew(xml_);
  free(xml_);
  NanReturnValue(xml);
}

NAN_METHOD(StorageVolume::LookupByName)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string to call this function");
    NanReturnUndefined();
  }

  Local<Object> pool_obj = args.This();
  if (!NanHasInstance(StoragePool::constructor_template, pool_obj)) {
    NanThrowTypeError("You must specify a StoragePool instance");
    NanReturnUndefined();
  }

  String::Utf8Value name(args[0]->ToString());
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(pool_obj);
  StorageVolume *volume = new StorageVolume();
  volume->volume_ = virStorageVolLookupByName(pool->Pool(), (const char *) *name);
  if (volume->volume_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> volume_obj = constructor_template->GetFunction()->NewInstance();
  volume->Wrap(volume_obj);
  NanReturnValue(volume_obj);
}

NAN_METHOD(StorageVolume::LookupByKey)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string to call this function");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value key(args[0]->ToString());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  StorageVolume *volume = new StorageVolume();
  volume->volume_ = virStorageVolLookupByKey(hypervisor->Connection(), (const char *) *key);
  if (volume->volume_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> volume_obj = constructor_template->GetFunction()->NewInstance();
  volume->Wrap(volume_obj);
  NanReturnValue(volume_obj);
}

NAN_METHOD(StorageVolume::LookupByPath)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string to call this function");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value path(args[0]->ToString());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  StorageVolume *volume = new StorageVolume();
  volume->volume_ = virStorageVolLookupByPath(hypervisor->Connection(), (const char *) *path);
  if (volume->volume_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> volume_obj = constructor_template->GetFunction()->NewInstance();
  volume->Wrap(volume_obj);
  NanReturnValue(volume_obj);
}

NAN_METHOD(StorageVolume::Clone)
{
  NanScope();

  unsigned int flags = 0;
  if (args.Length() < 2) {
    NanThrowTypeError("You must specify two arguments to call this function");
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

  Local<Object> pool_obj = args.This();
  if (!NanHasInstance(StoragePool::constructor_template, pool_obj)) {
    NanThrowTypeError("You must specify a StoragePool instance");
    NanReturnUndefined();
  }

  String::Utf8Value xml(args[1]->ToString());
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(pool_obj);
  StorageVolume *source_volume = ObjectWrap::Unwrap<StorageVolume>(args[0]->ToObject());
  StorageVolume *clone_volume = new StorageVolume();
  clone_volume->volume_ =
    virStorageVolCreateXMLFrom(pool->Pool(), (const char *) *xml, source_volume->volume_, flags);
  if (clone_volume->volume_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> clone_vol_obj = constructor_template->GetFunction()->NewInstance();
  clone_volume->Wrap(clone_vol_obj);
  NanReturnValue(clone_vol_obj);
}

} //namespace NodeLibvirt

