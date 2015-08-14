// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "error.h"
#include "hypervisor.h"
#include "network.h"

namespace NodeLibvirt {

Persistent<Function> Network::constructor;
void Network::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = NanNew<FunctionTemplate>();
  t->SetClassName(NanNew("Network"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "start",           Start);
  NODE_SET_PROTOTYPE_METHOD(t, "getName",         GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getUUID",         GetUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "getAutostart",    GetAutostart);
  NODE_SET_PROTOTYPE_METHOD(t, "setAutostart",    SetAutostart);
  NODE_SET_PROTOTYPE_METHOD(t, "isActive",        IsActive);
  NODE_SET_PROTOTYPE_METHOD(t, "isPersistent",    IsPersistent);
  NODE_SET_PROTOTYPE_METHOD(t, "undefine",        Undefine);
  NODE_SET_PROTOTYPE_METHOD(t, "destroy",         Destroy);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml",           ToXml);
  NODE_SET_PROTOTYPE_METHOD(t, "getBridgeName",   GetBridgeName);

  NanAssignPersistent(constructor, t->GetFunction());
  exports->Set(NanNew("Network"), t->GetFunction());
}

Local<Object> Network::NewInstance(virNetworkPtr handle)
{
  NanEscapableScope();
  Local<Function> ctor = NanNew<Function>(constructor);
  Local<Object> object = ctor->NewInstance();

  Network *network = new Network(handle);
  network->Wrap(object);
  return NanEscapeScope(object);
}

Network::~Network()
{
  if (handle_ != NULL)
    virNetworkFree(handle_);
  handle_ = 0;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Network, LookupByName, virNetworkLookupByName)
NAN_METHOD(Network::LookupByName)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid network name and callback.");
    NanReturnUndefined();
  }

  if (!NanHasInstance(Hypervisor::constructor_template, args.This())) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  std::string name(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByNameWorker(callback, hv->handle_, name));
  NanReturnUndefined();
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Network, LookupByUUID, virNetworkLookupByUUIDString)
NAN_METHOD(Network::LookupByUUID)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid network uuid and callback.");
    NanReturnUndefined();
  }

  if (!NanHasInstance(Hypervisor::constructor_template, args.This())) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  std::string uuid(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByUUIDWorker(callback, hv->handle_, uuid));
  NanReturnUndefined();
}

NLV_WORKER_METHOD_CREATE(Network)
NLV_WORKER_EXECUTE(Network, Create)
{
  NLV_WORKER_ASSERT_CONNECTION();
  lookupHandle_ = virNetworkCreateXML(Handle(), value_.c_str());
  if (lookupHandle_ == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_DEFINE(Network)
NLV_WORKER_EXECUTE(Network, Define)
{
  NLV_WORKER_ASSERT_CONNECTION();
  lookupHandle_ = virNetworkDefineXML(Handle(), value_.c_str());
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
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a bool and callback");
    NanReturnUndefined();
  }

  bool autoStart = args[0]->IsTrue();
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  NanAsyncQueueWorker(new SetAutostartWorker(callback, network->handle_, autoStart));
  NanReturnUndefined();
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

}   // namespace NodeLibvirt
