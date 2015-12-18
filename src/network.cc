// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "error.h"
#include "hypervisor.h"
#include "network.h"

namespace NLV {

Nan::Persistent<Function> Network::constructor;
void Network::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>();
  t->SetClassName(Nan::New("Network").ToLocalChecked());
  t->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(t, "start",           Start);
  Nan::SetPrototypeMethod(t, "getName",         GetName);
  Nan::SetPrototypeMethod(t, "getUUID",         GetUUID);
  Nan::SetPrototypeMethod(t, "getAutostart",    GetAutostart);
  Nan::SetPrototypeMethod(t, "setAutostart",    SetAutostart);
  Nan::SetPrototypeMethod(t, "isActive",        IsActive);
  Nan::SetPrototypeMethod(t, "isPersistent",    IsPersistent);
  Nan::SetPrototypeMethod(t, "undefine",        Undefine);
  Nan::SetPrototypeMethod(t, "destroy",         Destroy);
  Nan::SetPrototypeMethod(t, "toXml",           ToXml);
  Nan::SetPrototypeMethod(t, "getBridgeName",   GetBridgeName);

  constructor.Reset(t->GetFunction());
  exports->Set(Nan::New("Network").ToLocalChecked(), t->GetFunction());
}

Network::Network(virNetworkPtr handle) : NLVObject(handle) {}
Local<Object> Network::NewInstance(virNetworkPtr handle)
{
  Nan::EscapableHandleScope scope;
  Local<Function> ctor = Nan::New<Function>(constructor);
  Local<Object> object = ctor->NewInstance();

  Network *network = new Network(handle);
  network->Wrap(object);
  return scope.Escape(object);
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Network, LookupByName, virNetworkLookupByName)
NAN_METHOD(Network::LookupByName)
{
  Nan::HandleScope scope;;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a valid network name and callback.");
    return;
  }

  if (!Nan::New(Hypervisor::constructor_template)->HasInstance(info.This())) {
    Nan::ThrowTypeError("You must specify a Hypervisor instance");
    return;
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(info.This());
  std::string name(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  NLV::AsyncQueueWorker(new LookupByNameWorker(callback, hv, name), info.This());
  return;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Network, LookupByUUID, virNetworkLookupByUUIDString)
NAN_METHOD(Network::LookupByUUID)
{
  Nan::HandleScope scope;;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a valid network uuid and callback.");
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

NLV_WORKER_METHOD_CREATE(Network)
NLV_WORKER_EXECUTE(Network, Create)
{
  NLV_WORKER_ASSERT_PARENT_HANDLE();
  lookupHandle_ = virNetworkCreateXML(parent_->handle_, value_.c_str());
  if (lookupHandle_ == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_DEFINE(Network)
NLV_WORKER_EXECUTE(Network, Define)
{
  NLV_WORKER_ASSERT_PARENT_HANDLE();
  lookupHandle_ = virNetworkDefineXML(parent_->handle_, value_.c_str());
  if (lookupHandle_ == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(Network, Start)
NLV_WORKER_EXECUTE(Network, Start)
{
  NLV_WORKER_ASSERT_NETWORK();
  int result = virNetworkCreate(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Network, GetName)
NLV_WORKER_EXECUTE(Network, GetName)
{
  NLV_WORKER_ASSERT_NETWORK();
  const char *result = virNetworkGetName(Handle());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Network, GetUUID)
NLV_WORKER_EXECUTE(Network, GetUUID)
{
  NLV_WORKER_ASSERT_NETWORK();
  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  int result = virNetworkGetUUIDString(Handle(), uuid);
  if (result == -1) {
    SetVirError(virGetLastError());
    delete[] uuid;
    return;
  }

  data_ = uuid;
  delete[] uuid;
}

NLV_WORKER_METHOD_NO_ARGS(Network, GetAutostart)
NLV_WORKER_EXECUTE(Network, GetAutostart)
{
  NLV_WORKER_ASSERT_NETWORK();
  int autostart;
  int result = virNetworkGetAutostart(Handle(), &autostart);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(autostart);
}

NAN_METHOD(Network::SetAutostart)
{
  Nan::HandleScope scope;;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a bool and callback");
    return;
  }

  bool autoStart = info[0]->IsTrue();
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Network *network = ObjectWrap::Unwrap<Network>(info.This());
  Nan::AsyncQueueWorker(new SetAutostartWorker(callback, network->handle_, autoStart));
  return;
}

NLV_WORKER_EXECUTE(Network, SetAutostart)
{
  NLV_WORKER_ASSERT_NETWORK();
  int result = virNetworkSetAutostart(Handle(), autoStart_ ? 1 : 0);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Network, IsActive)
NLV_WORKER_EXECUTE(Network, IsActive)
{
  NLV_WORKER_ASSERT_NETWORK();
  int result = virNetworkIsActive(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_NO_ARGS(Network, IsPersistent)
NLV_WORKER_EXECUTE(Network, IsPersistent)
{
  NLV_WORKER_ASSERT_NETWORK();
  int result = virNetworkIsPersistent(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_NO_ARGS(Network, Undefine)
NLV_WORKER_EXECUTE(Network, Undefine)
{
  NLV_WORKER_ASSERT_NETWORK();
  int result = virNetworkUndefine(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Network, Destroy)
NLV_WORKER_EXECUTE(Network, Destroy)
{
  NLV_WORKER_ASSERT_NETWORK();
  int result = virNetworkDestroy(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Network, ToXml)
NLV_WORKER_EXECUTE(Network, ToXml)
{
  NLV_WORKER_ASSERT_NETWORK();
  unsigned int flags = 0;
  char *result = virNetworkGetXMLDesc(Handle(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_WORKER_METHOD_NO_ARGS(Network, GetBridgeName)
NLV_WORKER_EXECUTE(Network, GetBridgeName)
{
  NLV_WORKER_ASSERT_NETWORK();
  const char *result = virNetworkGetBridgeName(Handle());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

}   // namespace NLV
