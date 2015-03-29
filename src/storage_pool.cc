// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

#include "storage_volume.h"
#include "storage_pool.h"
//FIXME default params, default flags

namespace NodeLibvirt {

Persistent<FunctionTemplate> StoragePool::constructor_template;
void StoragePool::Initialize()
{
  NanScope();
  Local<FunctionTemplate> t = FunctionTemplate::New();
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "build",               Build);
  NODE_SET_PROTOTYPE_METHOD(t, "start",               Start);
  NODE_SET_PROTOTYPE_METHOD(t, "stop",                Stop);
  NODE_SET_PROTOTYPE_METHOD(t, "erase",               Erase);
  NODE_SET_PROTOTYPE_METHOD(t, "setAutostart",        SetAutostart);
  NODE_SET_PROTOTYPE_METHOD(t, "getAutostart",        GetAutostart);
  NODE_SET_PROTOTYPE_METHOD(t, "getInfo",             GetInfo);
  NODE_SET_PROTOTYPE_METHOD(t, "getName",             GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getUUID",             GetUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "undefine",            Undefine);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml",               ToXml);
  NODE_SET_PROTOTYPE_METHOD(t, "isActive",            IsActive);
  NODE_SET_PROTOTYPE_METHOD(t, "isPersistent",        IsPersistent);
  NODE_SET_PROTOTYPE_METHOD(t, "getVolumes",          GetVolumes);
  NODE_SET_PROTOTYPE_METHOD(t, "refresh",             Refresh);
  NODE_SET_PROTOTYPE_METHOD(t, "createVolume",        StorageVolume::Create);
  NODE_SET_PROTOTYPE_METHOD(t, "cloneVolume",         StorageVolume::Clone);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupVolumeByName",  StorageVolume::LookupByName);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(String::NewSymbol("StoragePool"));

  Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

  //Constants initialization
  //virStoragePoolDeleteFlags
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_DELETE_NORMAL);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_DELETE_ZEROED);

  //virStoragePoolState
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_INACTIVE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_BUILDING);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_RUNNING);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_DEGRADED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_INACCESSIBLE);
}

virStoragePoolPtr StoragePool::Pool() const
{
  return pool_;
}

NAN_METHOD(StoragePool::Build)
{
  NanScope();

  unsigned int flags = 0;
  int ret = -1;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolBuild(pool->pool_, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(StoragePool::Create)
{
  NanScope();

  unsigned int flags = 0;
  if (args.Length() == 0) {
    NanThrowTypeError("You must specify at least one argument");
    NanReturnUndefined();
  }

  if (!args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as first argument");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor object instance");
    NanReturnUndefined();
  }

  String::Utf8Value xml(args[0]->ToString());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  StoragePool *pool = new StoragePool();
  pool->pool_ = virStoragePoolCreateXML(hypervisor->Connection(), (const char *) *xml, flags);
  if (pool->pool_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> pool_obj = constructor_template->GetFunction()->NewInstance();
  pool->Wrap(pool_obj);
  NanReturnValue(pool_obj);
}

NAN_METHOD(StoragePool::Define)
{
  NanScope();

  unsigned int flags = 0;
  if (args.Length() == 0) {
    NanThrowTypeError("You must specify at least one argument");
    NanReturnUndefined();
  }

  if (!args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as first argument");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value xml(args[0]->ToString());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  StoragePool *pool = new StoragePool();
  pool->pool_ = virStoragePoolDefineXML(hypervisor->Connection(), (const char *) *xml, flags);
  if (pool->pool_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> pool_obj = constructor_template->GetFunction()->NewInstance();
  pool->Wrap(pool_obj);
  NanReturnValue(pool_obj);
}

NAN_METHOD(StoragePool::Undefine)
{
  NanScope();

  int ret = -1;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolUndefine(pool->pool_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  if (pool->pool_ != NULL) {
    virStoragePoolFree(pool->pool_);
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(StoragePool::LookupByName)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as first argument to call this function");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value name(args[0]->ToString());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  StoragePool *pool = new StoragePool();
  pool->pool_ = virStoragePoolLookupByName(hypervisor->Connection(), (const char *) *name);
  if (pool->pool_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> pool_obj = constructor_template->GetFunction()->NewInstance();
  pool->Wrap(pool_obj);
  NanReturnValue(pool_obj);
}

NAN_METHOD(StoragePool::LookupByUUID)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a UUID string.");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value uuid(args[0]->ToString());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  StoragePool *pool = new StoragePool();
  pool->pool_ = virStoragePoolLookupByUUIDString(hypervisor->Connection(), (const char *) *uuid);
  if (pool->pool_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> pool_obj = constructor_template->GetFunction()->NewInstance();
  pool->Wrap(pool_obj);
  NanReturnValue(pool_obj);
}

NAN_METHOD(StoragePool::LookupByVolume)
{
  NanScope();
  NanReturnUndefined();
}

NAN_METHOD(StoragePool::Start)
{
  NanScope();

  unsigned int flags = 0;
  int ret = -1;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolCreate(pool->pool_, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(StoragePool::Stop)
{
    NanScope();

    int ret = -1;
    StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
    ret = virStoragePoolDestroy(pool->pool_);
    if (ret == -1) {
      ThrowException(Error::New(virGetLastError()));
      NanReturnValue(NanFalse());
    }

    // if (pool->pool_ != NULL) {
    //   virStoragePoolFree(pool->pool_);

    NanReturnValue(NanTrue());
}

NAN_METHOD(StoragePool::Erase)
{
  NanScope();

  unsigned int flags = 0;
  int ret = -1;
  if (args.Length() == 1) {
    if(!args[0]->IsArray()) {
      NanThrowTypeError("You must specify an array as argument to call this function");
      NanReturnUndefined();
    }

    //flags
    Local<Array> flags_ = Local<Array>::Cast(args[0]);
    unsigned int length = flags_->Length();
    for (unsigned int i = 0; i < length; i++) {
      flags |= flags_->Get(Integer::New(i))->Int32Value();
    }
  }

  if (flags == 0) {
    flags |= VIR_STORAGE_POOL_DELETE_NORMAL;
  }

  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolDelete(pool->pool_, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  if (pool->pool_ != NULL) {
    virStoragePoolFree(pool->pool_);
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(StoragePool::GetAutostart)
{
  NanScope();

  int ret = -1;
  int autostart_;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolGetAutostart(pool->pool_, &autostart_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(static_cast<bool>(autostart_)));
}

NAN_METHOD(StoragePool::SetAutostart)
{
  NanScope();

  int ret = -1;
  if (args.Length() == 0 || !args[0]->IsBoolean()) {
    NanThrowTypeError("You must specify a boolean argument");
    NanReturnUndefined();
  }

  bool autostart = args[0]->IsTrue();
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolSetAutostart(pool->pool_, autostart ? 0 : 1);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(StoragePool::GetInfo)
{
  NanScope();

  virStoragePoolInfo info;
  int ret = -1;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolGetInfo(pool->pool_, &info);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> object = NanNew<Object>();
  object->Set(NanNew("state"), NanNew<Integer>(info.state)); //virStoragePoolState
  object->Set(NanNew("capacity"), NanNew<Number>(info.capacity)); //bytes
  object->Set(NanNew("allocation"), NanNew<Number>(info.allocation)); //bytes
  object->Set(NanNew("available"), NanNew<Number>(info.available)); //bytes
  NanReturnValue(object);
}

NAN_METHOD(StoragePool::GetName)
{
  NanScope();

  const char *name = NULL;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  name = virStoragePoolGetName(pool->pool_);
  if (name == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(name));
}

NAN_METHOD(StoragePool::GetUUID)
{
  NanScope();

  int ret = -1;
  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolGetUUIDString(pool->pool_, uuid);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    delete[] uuid;
    NanReturnUndefined();
  }

  Local<String> uuid_str = NanNew(uuid);
  delete[] uuid;
  NanReturnValue(uuid_str);
}

NAN_METHOD(StoragePool::ToXml)
{
  NanScope();

  char* xml_ = NULL;
  int flags = 0;
  if (args.Length() == 0 || !args[0]->IsArray()) {
    NanThrowTypeError("You must specify an array as argument to invoke this function");
    NanReturnUndefined();
  }

  //flags
  Local<Array> flags_ = Local<Array>::Cast(args[0]);
  unsigned int length = flags_->Length();
  for (unsigned int i = 0; i < length; i++) {
    flags |= flags_->Get(Integer::New(i))->Int32Value();
  }

  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  xml_ = virStoragePoolGetXMLDesc(pool->pool_, flags);
  if (xml_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<String> xml = NanNew(xml_);
  free(xml_);
  NanReturnValue(xml);
}

NAN_METHOD(StoragePool::IsActive)
{
  NanScope();

  int ret = -1;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolIsActive(pool->pool_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(static_cast<bool>(ret)));
}

NAN_METHOD(StoragePool::IsPersistent)
{
  NanScope();

  int ret = -1;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolIsPersistent(pool->pool_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(static_cast<bool>(ret)));
}

NAN_METHOD(StoragePool::GetVolumes)
{
  NanScope();

  char **volumes_ = NULL;
  int num_volumes = 0;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  num_volumes = virStoragePoolNumOfVolumes(pool->pool_);
  if (num_volumes == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  volumes_ = (char**) malloc(sizeof(*volumes_) * num_volumes);
  if (volumes_ == NULL) {
    LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
    NanReturnUndefined();
  }

  num_volumes = virStoragePoolListVolumes(pool->pool_, volumes_, num_volumes);
  if (num_volumes == -1) {
    free(volumes_);
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Array> volumes = Array::New(num_volumes);
  for (int i = 0; i < num_volumes; i++) {
    //TODO new StorageVolume and return array of StorageVolume instances
    volumes->Set(NanNew<Integer>(i), NanNew<String>(volumes_[i]));
    free(volumes_[i]);
  }

  free(volumes_);
  NanReturnValue(volumes);
}

NAN_METHOD(StoragePool::Refresh)
{
  NanScope();

  unsigned int flags = 0;
  int ret = -1;
  StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
  ret = virStoragePoolRefresh(pool->pool_, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

} //namespace NodeLibvirt

