// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

#include "error.h"
#include "hypervisor.h"
#include "node_device.h"

using namespace v8;

namespace NodeLibvirt {

Persistent<FunctionTemplate> NodeDevice::constructor_template;
void NodeDevice::Initialize()
{
  Local<FunctionTemplate> t = FunctionTemplate::New();
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "destroy",           Destroy);
  NODE_SET_PROTOTYPE_METHOD(t, "detach",            Detach);
  NODE_SET_PROTOTYPE_METHOD(t, "reattach",          Reattach);
  NODE_SET_PROTOTYPE_METHOD(t, "reset",             Destroy);
  NODE_SET_PROTOTYPE_METHOD(t, "getName",           GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getParentName",     GetParentName);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml",             ToXml);
  NODE_SET_PROTOTYPE_METHOD(t, "getCapabilities",   GetCapabilities);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(NanNew("NodeDevice"));
}

Local<Object> NodeDevice::NewInstance(const LibVirtHandle &handle)
{
  NanScope();
  NodeDevice *nodeDevice = new NodeDevice(handle.ToNodeDevice());
  Local<Object> object = constructor_template->GetFunction()->NewInstance();
  nodeDevice->Wrap(object);
  return NanEscapeScope(object);
}

NLV_LOOKUP_BY_VALUE_EXECUTE(NodeDevice, LookupByName, virNodeDeviceLookupByName)
NAN_METHOD(NodeDevice::LookupByName)
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

NAN_METHOD(NodeDevice::Create)
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

void NodeDevice::CreateWorker::Execute()
{
  unsigned int flags = 0;
  lookupHandle_ =
    virNodeDeviceCreateXML(Handle().ToConnection(), value_.c_str(), flags);
  if (lookupHandle_.ToNodeDevice() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

//Really neccesary call destroy from javascript ???
NLV_WORKER_METHOD_NO_ARGS(NodeDevice, Destroy)
void NodeDevice::DestroyWorker::Execute()
{
  NLV_WORKER_ASSERT_NODEDEVICE();

  int result = virNodeDeviceDestroy(Handle().ToNodeDevice());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  if (Handle().ToNodeDevice() != NULL) {
    Handle().Clear();
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, Detach)
void NodeDevice::DetachWorker::Execute()
{
  NLV_WORKER_ASSERT_NODEDEVICE();

  int result = virNodeDeviceDettach(Handle().ToNodeDevice());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, Reattach)
void NodeDevice::ReattachWorker::Execute()
{
  NLV_WORKER_ASSERT_NODEDEVICE();

  int result = virNodeDeviceReAttach(Handle().ToNodeDevice());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, Reset)
void NodeDevice::ResetWorker::Execute()
{
  NLV_WORKER_ASSERT_NODEDEVICE();

  int result = virNodeDeviceReset(Handle().ToNodeDevice());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, GetName)
void NodeDevice::GetNameWorker::Execute()
{
  NLV_WORKER_ASSERT_NODEDEVICE();

  const char *result = virNodeDeviceGetName(Handle().ToNodeDevice());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, GetParentName)
void NodeDevice::GetParentNameWorker::Execute()
{
  NLV_WORKER_ASSERT_NODEDEVICE();

  const char *result = virNodeDeviceGetParent(Handle().ToNodeDevice());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, ToXml)
void NodeDevice::ToXmlWorker::Execute()
{
  NLV_WORKER_ASSERT_NODEDEVICE();

  unsigned int flags = 0;
  char *result = virNodeDeviceGetXMLDesc(Handle().ToNodeDevice(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, GetCapabilities)
void NodeDevice::GetCapabilitiesWorker::Execute()
{
  NanScope();
  int num_caps = virNodeDeviceNumOfCaps(Handle().ToNodeDevice());
  if (num_caps == -1) {
    SetVirError(virGetLastError());
    return;
  }

  char **names = (char**) malloc(sizeof(*names) * num_caps);
  if (names == NULL) {
    SetErrorMessage("Unable to allocate memory");
    return;
  }

  num_caps = virNodeDeviceListCaps(Handle().ToNodeDevice(), names, num_caps);
  if (num_caps == -1) {
    free(names);
    SetVirError(virGetLastError());
    return;
  }

  for (int i = 0; i < num_caps; ++i) {
    data_.push_back(names[i]);
    free(names[i]);
  }

  free(names);
}

} //namespace NodeLibvirt
