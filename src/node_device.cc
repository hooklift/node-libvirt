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

  NODE_SET_PROTOTYPE_METHOD(t, "destroy", NodeDevice::Destroy);
  NODE_SET_PROTOTYPE_METHOD(t, "detach", NodeDevice::Detach);
  NODE_SET_PROTOTYPE_METHOD(t, "reattach", NodeDevice::Reattach);
  NODE_SET_PROTOTYPE_METHOD(t, "reset", NodeDevice::Destroy);
  NODE_SET_PROTOTYPE_METHOD(t, "getName", NodeDevice::GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getParentName", NodeDevice::GetParentName);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml", NodeDevice::ToXml);
  NODE_SET_PROTOTYPE_METHOD(t, "getCapabilities", NodeDevice::GetCapabilities);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(String::NewSymbol("NodeDevice"));
}

NAN_METHOD(NodeDevice::LookupByName)
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

  String::Utf8Value name(args[0]->ToString());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  NodeDevice *device = new NodeDevice();
  device->device_ = virNodeDeviceLookupByName(hypervisor->Connection(), (const char *) *name);
  if (device->device_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> device_obj = constructor_template->GetFunction()->NewInstance();
  device->Wrap(device_obj);
  NanReturnValue(device_obj);
}

NAN_METHOD(NodeDevice::Create)
{
  NanScope();

  unsigned int flags = 0;
  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string to call this function");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value xml(args[0]->ToString());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  NodeDevice *device = new NodeDevice();
  device->device_ = virNodeDeviceCreateXML(hypervisor->Connection(), (const char *) *xml, flags);
  if (device->device_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> device_obj = constructor_template->GetFunction()->NewInstance();
  device->Wrap(device_obj);
  NanReturnValue(device_obj);
}

//Really neccesary call destroy from javascript ???
NAN_METHOD(NodeDevice::Destroy)
{
  NanScope();

  int ret = -1;
  NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
  ret = virNodeDeviceDestroy(device->device_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(NodeDevice::Detach)
{
  NanScope();

  int ret = -1;
  NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
  ret = virNodeDeviceDettach(device->device_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

Handle<Value> NodeDevice::Reattach(const Arguments& args) {
    HandleScope scope;
    int ret = -1;

    NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
    ret = virNodeDeviceReAttach(device->device_);

    if(ret == -1) {
        ThrowException(Error::New(virGetLastError()));
        return False();
    }

    return True();
}

NAN_METHOD(NodeDevice::Reset)
{
  NanScope();

  int ret = -1;
  NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
  ret = virNodeDeviceReset(device->device_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(NodeDevice::GetName)
{
  NanScope();

  const char *name = NULL;
  NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
  name = virNodeDeviceGetName(device->device_);
  if (name == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(name));
}

NAN_METHOD(NodeDevice::GetParentName)
{
  NanScope();

  const char *name = NULL;
  NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
  name = virNodeDeviceGetParent(device->device_);
  if (name == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(name));
}

NAN_METHOD(NodeDevice::ToXml)
{
  NanScope();

  unsigned int flags = 0;
  const char *xml = NULL;
  NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
  xml = virNodeDeviceGetXMLDesc(device->device_, flags);
  if (xml == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(xml));
}

NAN_METHOD(NodeDevice::GetCapabilities)
{
  NanScope();

  char **names_ = NULL;
  int numcaps = -1;
  NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
  numcaps = virNodeDeviceNumOfCaps(device->device_);
  if (numcaps == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  names_ = (char**) malloc(sizeof(*names_) * numcaps);
  if (names_ == NULL) {
    LIBVIRT_THROW_EXCEPTION("Unable to allocate memory");
    NanReturnUndefined();
  }

  numcaps = virNodeDeviceListCaps(device->device_, names_, numcaps);
  if (numcaps == -1) {
    free(names_);
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Array> names = Array::New(numcaps);
  for (int i = 0; i < numcaps; ++i) {
    names->Set(NanNew(i), NanNew(names_[i]));
    free(names_[i]);
  }

  free(names_);
  NanReturnValue(names);
}

} //namespace NodeLibvirt
