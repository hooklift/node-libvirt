// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

#include "error.h"
#include "hypervisor.h"
#include "network_filter.h"

namespace NodeLibvirt {

Persistent<FunctionTemplate> NetworkFilter::constructor_template;
void NetworkFilter::Initialize() {
    Local<FunctionTemplate> t = FunctionTemplate::New();
    t->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(t, "getName", NetworkFilter::GetName);
    NODE_SET_PROTOTYPE_METHOD(t, "getUUID", NetworkFilter::GetUUID);
    NODE_SET_PROTOTYPE_METHOD(t, "undefine", NetworkFilter::Undefine);
    NODE_SET_PROTOTYPE_METHOD(t, "toXml", NetworkFilter::ToXml);

    NanAssignPersistent(constructor_template, t);
    constructor_template->SetClassName(NanNew("NetworkFilter"));
}

NAN_METHOD(NetworkFilter::LookupByName)
{
  NanScope();

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a valid Network Filter name.");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  String::Utf8Value name(args[0]->ToString());

  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

  NetworkFilter *nwfilter = new NetworkFilter();
  nwfilter->filter_ = virNWFilterLookupByName(hypervisor->Connection(), (const char *) *name);
  if (nwfilter->filter_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> nwfilter_obj = constructor_template->GetFunction()->NewInstance();
  nwfilter->Wrap(nwfilter_obj);
  NanReturnValue(nwfilter_obj);
}

NAN_METHOD(NetworkFilter::LookupByUUID)
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
  NetworkFilter *nwfilter = new NetworkFilter();
  nwfilter->filter_ = virNWFilterLookupByUUIDString(hypervisor->Connection(), (const char *) *uuid);
  if (nwfilter->filter_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> nwfilter_obj = constructor_template->GetFunction()->NewInstance();
  nwfilter->Wrap(nwfilter_obj);
  NanReturnValue(nwfilter_obj);
}

NAN_METHOD(NetworkFilter::Define)
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
  NetworkFilter *nwfilter = new NetworkFilter();
  nwfilter->filter_ = virNWFilterDefineXML(hypervisor->Connection(), (const char *) *xml);
  if (nwfilter->filter_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> nwfilter_obj = constructor_template->GetFunction()->NewInstance();
  nwfilter->Wrap(nwfilter_obj);
  NanReturnValue(nwfilter_obj);
}

NAN_METHOD(NetworkFilter::GetName)
{
  NanScope();

  const char *name = NULL;
  NetworkFilter *nwfilter = ObjectWrap::Unwrap<NetworkFilter>(args.This());
  name = virNWFilterGetName(nwfilter->filter_);
  if (name == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(name));
}

NAN_METHOD(NetworkFilter::GetUUID)
{
  NanScope();

  int ret = -1;
  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  NetworkFilter *nwfilter = ObjectWrap::Unwrap<NetworkFilter>(args.This());
  ret = virNWFilterGetUUIDString(nwfilter->filter_, uuid);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    delete[] uuid;
    NanReturnUndefined();
  }

  Local<String> uuid_str = NanNew(uuid);
  delete[] uuid;
  NanReturnValue(uuid_str);
}

NAN_METHOD(NetworkFilter::Undefine)
{
  NanScope();

  int ret = -1;
  NetworkFilter *nwfilter = ObjectWrap::Unwrap<NetworkFilter>(args.This());
  ret = virNWFilterUndefine(nwfilter->filter_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(NetworkFilter::ToXml)
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

  NetworkFilter *nwfilter = ObjectWrap::Unwrap<NetworkFilter>(args.This());
  xml_ = virNWFilterGetXMLDesc(nwfilter->filter_, flags);
  if (xml_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<String> xml = NanNew(xml_);
  free(xml_);
  NanReturnValue(xml);
}

}   // namespce NodeLibvirt

