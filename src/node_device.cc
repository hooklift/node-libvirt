// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

#include "error.h"
#include "hypervisor.h"
#include "node_device.h"

using namespace v8;

namespace NLV {

Nan::Persistent<Function> NodeDevice::constructor;
void NodeDevice::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>();
  t->SetClassName(Nan::New("NodeDevice").ToLocalChecked());
  t->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(t, "destroy",           Destroy);
  Nan::SetPrototypeMethod(t, "detach",            Detach);
  Nan::SetPrototypeMethod(t, "reattach",          Reattach);
  Nan::SetPrototypeMethod(t, "reset",             Destroy);
  Nan::SetPrototypeMethod(t, "getName",           GetName);
  Nan::SetPrototypeMethod(t, "getParentName",     GetParentName);
  Nan::SetPrototypeMethod(t, "toXml",             ToXml);
  Nan::SetPrototypeMethod(t, "getCapabilities",   GetCapabilities);

  constructor.Reset(t->GetFunction());
  exports->Set(Nan::New("NodeDevice").ToLocalChecked(), t->GetFunction());
}

NodeDevice::NodeDevice(virNodeDevicePtr handle) : NLVObject(handle) {}
Local<Object> NodeDevice::NewInstance(virNodeDevicePtr handle)
{
  Nan::EscapableHandleScope scope;
  Local<Function> ctor = Nan::New<Function>(constructor);
  Local<Object> object = ctor->NewInstance();

  NodeDevice *nodeDevice = new NodeDevice(handle);
  nodeDevice->Wrap(object);
  return scope.Escape(object);
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(NodeDevice, LookupByName, virNodeDeviceLookupByName)
NAN_METHOD(NodeDevice::LookupByName)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a valid network name and callback.");
    return;
  }

  if (!Nan::New(Hypervisor::constructor_template)->HasInstance(info.This())) {
    Nan::ThrowTypeError("You must specify a Hypervisor instance");
    return;
  }

  Hypervisor *hv = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  std::string name(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Nan::AsyncQueueWorker(new LookupByNameWorker(callback, hv, name));
  return;
}

NAN_METHOD(NodeDevice::Create)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a string and callback");
    return;
  }

  if (!Nan::New(Hypervisor::constructor_template)->HasInstance(info.This())) {
    Nan::ThrowTypeError("You must specify a Hypervisor instance");
    return;
  }

  Hypervisor *hv = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  std::string xmlData(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Nan::AsyncQueueWorker(new CreateWorker(callback, hv, xmlData));
  return;
}

NLV_WORKER_EXECUTE(NodeDevice, Create)
{
  NLV_WORKER_ASSERT_PARENT_HANDLE();
  unsigned int flags = 0;
  lookupHandle_ = virNodeDeviceCreateXML(parent_->handle_, value_.c_str(), flags);
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

  Nan::HandleScope scope;
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

} //namespace NLV
