// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

#include "error.h"
#include "hypervisor.h"
#include "node_device.h"

using namespace v8;

namespace NodeLibvirt {

Persistent<Function> NodeDevice::constructor;
void NodeDevice::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = NanNew<FunctionTemplate>();
  t->SetClassName(NanNew("NodeDevice"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "destroy",           Destroy);
  NODE_SET_PROTOTYPE_METHOD(t, "detach",            Detach);
  NODE_SET_PROTOTYPE_METHOD(t, "reattach",          Reattach);
  NODE_SET_PROTOTYPE_METHOD(t, "reset",             Destroy);
  NODE_SET_PROTOTYPE_METHOD(t, "getName",           GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getParentName",     GetParentName);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml",             ToXml);
  NODE_SET_PROTOTYPE_METHOD(t, "getCapabilities",   GetCapabilities);

  NanAssignPersistent(constructor, t->GetFunction());
  exports->Set(NanNew("NodeDevice"), t->GetFunction());
}

Local<Object> NodeDevice::NewInstance(virNodeDevicePtr handle)
{
  NanEscapableScope();
  Local<Function> ctor = NanNew<Function>(constructor);
  Local<Object> object = ctor->NewInstance();

  NodeDevice *nodeDevice = new NodeDevice(handle);
  nodeDevice->Wrap(object);
  return NanEscapeScope(object);
}


NodeDevice::~NodeDevice()
{
  if (handle_ != NULL)
    virNodeDeviceFree(handle_);
  handle_ = 0;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(NodeDevice, LookupByName, virNodeDeviceLookupByName)
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

NLV_WORKER_EXECUTE(NodeDevice, Create)
{
  NLV_WORKER_ASSERT_CONNECTION();
  unsigned int flags = 0;
  lookupHandle_ = virNodeDeviceCreateXML(Handle(), value_.c_str(), flags);
  if (lookupHandle_ == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, Destroy)
NLV_WORKER_EXECUTE(NodeDevice, Destroy)
{
  NLV_WORKER_ASSERT_NODEDEVICE();
  int result = virNodeDeviceDestroy(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, Detach)
NLV_WORKER_EXECUTE(NodeDevice, Detach)
{
  NLV_WORKER_ASSERT_NODEDEVICE();
  int result = virNodeDeviceDettach(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, Reattach)
NLV_WORKER_EXECUTE(NodeDevice, Reattach)
{
  NLV_WORKER_ASSERT_NODEDEVICE();
  int result = virNodeDeviceReAttach(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, Reset)
NLV_WORKER_EXECUTE(NodeDevice, Reset)
{
  NLV_WORKER_ASSERT_NODEDEVICE();
  int result = virNodeDeviceReset(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, GetName)
NLV_WORKER_EXECUTE(NodeDevice, GetName)
{
  NLV_WORKER_ASSERT_NODEDEVICE();
  const char *result = virNodeDeviceGetName(Handle());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, GetParentName)
NLV_WORKER_EXECUTE(NodeDevice, GetParentName)
{
  NLV_WORKER_ASSERT_NODEDEVICE();
  const char *result = virNodeDeviceGetParent(Handle());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, ToXml)
NLV_WORKER_EXECUTE(NodeDevice, ToXml)
{
  NLV_WORKER_ASSERT_NODEDEVICE();
  unsigned int flags = 0;
  char *result = virNodeDeviceGetXMLDesc(Handle(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_WORKER_METHOD_NO_ARGS(NodeDevice, GetCapabilities)
NLV_WORKER_EXECUTE(NodeDevice, GetCapabilities)
{
  NLV_WORKER_ASSERT_NODEDEVICE();

  NanScope();
  int num_caps = virNodeDeviceNumOfCaps(Handle());
  if (num_caps == -1) {
    SetVirError(virGetLastError());
    return;
  }

  char **names = (char**) malloc(sizeof(*names) * num_caps);
  if (names == NULL) {
    SetErrorMessage("Unable to allocate memory");
    return;
  }

  num_caps = virNodeDeviceListCaps(Handle(), names, num_caps);
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
