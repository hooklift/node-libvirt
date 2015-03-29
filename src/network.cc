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

  NODE_SET_PROTOTYPE_METHOD(t, "start", Network::Start);
  NODE_SET_PROTOTYPE_METHOD(t, "getName", Network::GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getUUID", Network::GetUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "getAutostart", Network::GetAutostart);
  NODE_SET_PROTOTYPE_METHOD(t, "setAutostart", Network::SetAutostart);
  NODE_SET_PROTOTYPE_METHOD(t, "isActive", Network::IsActive);
  NODE_SET_PROTOTYPE_METHOD(t, "isPersistent", Network::IsPersistent);
  NODE_SET_PROTOTYPE_METHOD(t, "undefine", Network::Undefine);
  NODE_SET_PROTOTYPE_METHOD(t, "destroy", Network::Destroy);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml", Network::ToXml);
  NODE_SET_PROTOTYPE_METHOD(t, "getBridgeName", Network::GetBridgeName);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(NanNew("Network"));
}

NAN_METHOD(Network::Create)
{
  NanScope();

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor object instance");
    NanReturnUndefined();
  }

  if (args.Length() == 1 && !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as first argument");
    NanReturnUndefined();
  }

  String::Utf8Value xml(args[0]->ToString());

  Network *network = new Network();
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  network->network_ = virNetworkCreateXML(hypervisor->Connection(), (const char *) *xml);
  if (network->network_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> network_obj = constructor_template->GetFunction()->NewInstance();
  network->Wrap(network_obj);
  NanReturnValue(network_obj);
}

NAN_METHOD(Network::Start)
{
  NanScope();

  int ret = -1;
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  ret = virNetworkCreate(network->network_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Network::LookupByName)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a valid Network name.");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();

  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value name(args[0]->ToString());

  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

  Network *network = new Network();
  network->network_ = virNetworkLookupByName(hypervisor->Connection(), (const char *) *name);
  if (network->network_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> network_obj = constructor_template->GetFunction()->NewInstance();
  network->Wrap(network_obj);
  NanReturnValue(network_obj);
}

NAN_METHOD(Network::LookupByUUID)
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

  Network *network = new Network();
  network->network_ = virNetworkLookupByUUIDString(hypervisor->Connection(), (const char *) *uuid);
  if (network->network_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> network_obj = constructor_template->GetFunction()->NewInstance();
  network->Wrap(network_obj);
  NanReturnValue(network_obj);
}

NAN_METHOD(Network::Define)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as argument to call this function");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value xml(args[0]->ToString());

  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

  Network *network = new Network();
  network->network_ = virNetworkDefineXML(hypervisor->Connection(), (const char *) *xml);
  if (network->network_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> network_obj = constructor_template->GetFunction()->NewInstance();
  network->Wrap(network_obj);
  NanReturnValue(network_obj);
}

NAN_METHOD(Network::GetName)
{
  NanScope();

  const char *name = NULL;
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  name = virNetworkGetName(network->network_);
  if (name == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(name));
}

NAN_METHOD(Network::GetUUID)
{
  NanScope();

  int ret = -1;
  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  ret = virNetworkGetUUIDString(network->network_, uuid);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    delete[] uuid;
    NanReturnUndefined();
  }

  Local<String> uuid_str = NanNew(uuid);
  delete[] uuid;
  NanReturnValue(uuid_str);
}

NAN_METHOD(Network::GetAutostart)
{
  NanScope();

  int ret = -1;
  int autostart_;

  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  ret = virNetworkGetAutostart(network->network_, &autostart_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(static_cast<bool>(autostart_)));
}

NAN_METHOD(Network::SetAutostart)
{
  NanScope();

  int ret = -1;
  if (args.Length() == 0 || !args[0]->IsBoolean()) {
    NanThrowTypeError("You must specify a boolean argument");
    NanReturnUndefined();
  }

  bool autostart = args[0]->IsTrue();
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  ret = virNetworkSetAutostart(network->network_, autostart ? 0 : 1);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Network::IsActive)
{
  NanScope();

  int ret = -1;
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  ret = virNetworkIsActive(network->network_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(static_cast<bool>(ret)));
}

NAN_METHOD(Network::IsPersistent)
{
  NanScope();

  int ret = -1;
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  ret = virNetworkIsPersistent(network->network_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(static_cast<bool>(ret)));
}

NAN_METHOD(Network::Undefine)
{
  NanScope();

  int ret = -1;
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  ret = virNetworkUndefine(network->network_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

// NOTE: Really neccesary call destroy from javascript ???
NAN_METHOD(Network::Destroy)
{
  NanScope();

  int ret = -1;
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  ret = virNetworkDestroy(network->network_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  if (network->network_ != NULL) {
    virNetworkFree(network->network_);
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Network::ToXml)
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

  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  xml_ = virNetworkGetXMLDesc(network->network_, flags);
  if (xml_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<String> xml = NanNew(xml_);
  free(xml_);
  NanReturnValue(xml);
}

NAN_METHOD(Network::GetBridgeName)
{
  NanScope();

  const char *name = NULL;
  Network *network = ObjectWrap::Unwrap<Network>(args.This());
  name = virNetworkGetBridgeName(network->network_);
  if (name == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(name));
}

}   // namespace NodeLibvirt
