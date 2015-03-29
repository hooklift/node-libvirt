// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>

#include "hypervisor.h"
#include "error.h"

#include "interface.h"

namespace NodeLibvirt {

Persistent<FunctionTemplate> Interface::constructor_template;
void Interface::Initialize()
{
  Local<FunctionTemplate> t = FunctionTemplate::New();
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "start", Interface::Start);
  NODE_SET_PROTOTYPE_METHOD(t, "stop", Interface::Stop);
  NODE_SET_PROTOTYPE_METHOD(t, "getName", Interface::GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getMacAddress", Interface::GetMacAddress);
  NODE_SET_PROTOTYPE_METHOD(t, "isActive", Interface::IsActive);
  NODE_SET_PROTOTYPE_METHOD(t, "undefine", Interface::Undefine);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml", Interface::ToXml);

  Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

  //Constants
  //virInterfaceXMLFlags
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_INTERFACE_XML_INACTIVE);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(NanNew("Interface"));
}

NAN_METHOD(Interface::Start)
{
  NanScope();

  int ret = -1;
  unsigned int flags = 0;

  Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());
  ret = virInterfaceCreate(interface->interface_, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Interface::Stop)
{
  NanScope();

  int ret = -1;
  unsigned int flags = 0;

  Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());
  ret = virInterfaceDestroy(interface->interface_, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  if (interface->interface_ != NULL) {
    virInterfaceFree(interface->interface_);
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Interface::Define)
{
  NanScope();

  unsigned int flags = 0;

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

  Interface *interface = new Interface();
  interface->interface_ =
    virInterfaceDefineXML(hypervisor->Connection(), (const char *) *xml, flags);

  if (interface->interface_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> interface_obj =
    constructor_template->GetFunction()->NewInstance();
  interface->Wrap(interface_obj);
  NanReturnValue(interface_obj);
}

NAN_METHOD(Interface::Undefine)
{
  NanScope();

  int ret = -1;

  Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());
  ret = virInterfaceUndefine(interface->interface_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Interface::LookupByName)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a valid Interface name.");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value name(args[0]->ToString());

  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

  Interface *interface = new Interface();
  interface->interface_ =
    virInterfaceLookupByName(hypervisor->Connection(), (const char *) *name);

  if (interface->interface_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnNull();
  }

  Local<Object> interface_obj =
    constructor_template->GetFunction()->NewInstance();
  interface->Wrap(interface_obj);
  NanReturnValue(interface_obj);
}

NAN_METHOD(Interface::LookupByMacAddress)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::TypeError(
    String::New("You must specify a string as argument to call this function")));
  }

  Local<Object> hyp_obj = args.This();

  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value mac(args[0]->ToString());

  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

  Interface *interface = new Interface();
  interface->interface_ =
    virInterfaceLookupByMACString(hypervisor->Connection(), (const char *) *mac);

  if (interface->interface_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> interface_obj =
    constructor_template->GetFunction()->NewInstance();
  interface->Wrap(interface_obj);
  NanReturnValue(interface_obj);
}

NAN_METHOD(Interface::GetName)
{
  NanScope();

  const char *name = NULL;
  Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());
  name = virInterfaceGetName(interface->interface_);
  if (name == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(name));
}

NAN_METHOD(Interface::GetMacAddress)
{
  NanScope();

  const char *mac = NULL;
  Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());
  mac = virInterfaceGetMACString(interface->interface_);
  if (mac == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(mac));
}

NAN_METHOD(Interface::IsActive)
{
  NanScope();

  int ret = -1;
  Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());
  ret = virInterfaceIsActive(interface->interface_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(static_cast<bool>(ret)));
}

NAN_METHOD(Interface::ToXml)
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

  Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());
  xml_ = virInterfaceGetXMLDesc(interface->interface_, flags);
  if (xml_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<String> xml = NanNew(xml_);
  free(xml_);
  NanReturnValue(xml);
}

}   // namespace NodeLibvirt