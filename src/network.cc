// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "error.h"
#include "hypervisor.h"
#include "network.h"

namespace NodeLibvirt {
Persistent<FunctionTemplate> Network::constructor_template;

void Network::Initialize()
{
  Local<FunctionTemplate> t = FunctionTemplate::New();
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

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(NanNew("Network"));
}

Local<Object> Network::NewInstance(const LibVirtHandle &handle)
{
  NanScope();
  Network *network = new Network(handle.ToNetwork());
  Local<Object> object = constructor_template->GetFunction()->NewInstance();
  network->Wrap(object);
  return NanEscapeScope(object);
}

NLV_LOOKUP_BY_VALUE_EXECUTE(Network, LookupByName, virNetworkLookupByName)
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

NLV_LOOKUP_BY_VALUE_EXECUTE(Network, LookupByUUID, virNetworkLookupByUUIDString)
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

NAN_METHOD(Network::Create)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a string and callback");
    NanReturnUndefined();
  }

  if (!NanHasInstance(Hypervisor::constructor_template, args.This())) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  std::string xmlData(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new CreateWorker(callback, hv->handle_, xmlData));
  NanReturnUndefined();
}

void Network::CreateWorker::Execute()
{
  lookupHandle_ =
    virNetworkCreateXML(Handle().ToConnection(), value_.c_str());
  if (lookupHandle_.ToNetwork() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NAN_METHOD(Network::Define)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a string and callback");
    NanReturnUndefined();
  }

  if (!NanHasInstance(Hypervisor::constructor_template, args.This())) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  std::string xmlData(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new DefineWorker(callback, hv->handle_, xmlData));
  NanReturnUndefined();
}

void Network::DefineWorker::Execute()
{
  lookupHandle_ =
    virNetworkDefineXML(Handle().ToConnection(), value_.c_str());
  if (lookupHandle_.ToNetwork() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(Network, Start)
void Network::StartWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  int result = virNetworkCreate(Handle().ToNetwork());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Network, GetName)
void Network::GetNameWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  const char *result = virNetworkGetName(Handle().ToNetwork());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Network, GetUUID)
void Network::GetUUIDWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  int result = virNetworkGetUUIDString(Handle().ToNetwork(), uuid);
  if (result == -1) {
    SetVirError(virGetLastError());
    delete[] uuid;
    return;
  }

  data_ = uuid;
  delete[] uuid;
}

NLV_WORKER_METHOD_NO_ARGS(Network, GetAutostart)
void Network::GetAutostartWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  int autostart;
  int result = virNetworkGetAutostart(Handle().ToNetwork(), &autostart);
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

void Network::SetAutostartWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  int result = virNetworkSetAutostart(Handle().ToNetwork(), autoStart_ ? 1 : 0);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Network, IsActive)
void Network::IsActiveWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  int result = virNetworkIsActive(Handle().ToNetwork());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_NO_ARGS(Network, IsPersistent)
void Network::IsPersistentWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  int result = virNetworkIsPersistent(Handle().ToNetwork());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_NO_ARGS(Network, Undefine)
void Network::UndefineWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  int result = virNetworkUndefine(Handle().ToNetwork());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Network, Destroy)
void Network::DestroyWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  int result = virNetworkDestroy(Handle().ToNetwork());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  if (Handle().ToNetwork() != NULL) {
    Handle().Clear();
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Network, ToXml)
void Network::ToXmlWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  unsigned int flags = 0;
  char *result = virNetworkGetXMLDesc(Handle().ToNetwork(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_WORKER_METHOD_NO_ARGS(Network, GetBridgeName)
void Network::GetBridgeNameWorker::Execute()
{
  NLV_WORKER_ASSERT_NETWORK();

  const char *result = virNetworkGetBridgeName(Handle().ToNetwork());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

}   // namespace NodeLibvirt
