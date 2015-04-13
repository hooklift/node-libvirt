// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

#include "error.h"
#include "hypervisor.h"
#include "network_filter.h"

namespace NodeLibvirt {

Persistent<FunctionTemplate> NetworkFilter::constructor_template;
void NetworkFilter::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = FunctionTemplate::New();
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "getName", NetworkFilter::GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getUUID", NetworkFilter::GetUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "undefine", NetworkFilter::Undefine);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml", NetworkFilter::ToXml);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(NanNew("NetworkFilter"));
  exports->Set(NanNew("NetworkFilter"), t->GetFunction());
}

Local<Object> NetworkFilter::NewInstance(const LibVirtHandle &handle)
{
  NanScope();
  NetworkFilter *filter = new NetworkFilter(handle.ToNetworkFilter());
  Local<Object> object = constructor_template->GetFunction()->NewInstance();
  filter->Wrap(object);
  return NanEscapeScope(object);
}

NetworkFilter::~NetworkFilter()
{
  if (handle_ != NULL)
    virNWFilterFree(handle_);
  handle_ = 0;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(NetworkFilter, LookupByName, virNWFilterLookupByName)
NAN_METHOD(NetworkFilter::LookupByName)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid network filter name and callback.");
    NanReturnUndefined();
  }

  Local<Object> object = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, object)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *unwrapped = ObjectWrap::Unwrap<Hypervisor>(object);
  std::string name(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByNameWorker(callback, unwrapped->handle_, name));
  NanReturnUndefined();
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(NetworkFilter, LookupByUUID, virNWFilterLookupByUUIDString)
NAN_METHOD(NetworkFilter::LookupByUUID)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid network filter uuid and callback.");
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

NLV_WORKER_METHOD_DEFINE(NetworkFilter)
NLV_WORKER_EXECUTE(NetworkFilter, Define)
{
  lookupHandle_ =
    virNWFilterDefineXML(Handle().ToConnection(), value_.c_str());
  if (lookupHandle_.ToNetworkFilter() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(NetworkFilter, GetName)
NLV_WORKER_EXECUTE(NetworkFilter, GetName)
{
  NLV_WORKER_ASSERT_INTERFACE();

  const char *result = virNWFilterGetName(Handle().ToNetworkFilter());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(NetworkFilter, GetUUID)
NLV_WORKER_EXECUTE(NetworkFilter, GetUUID)
{
  NLV_WORKER_ASSERT_INTERFACE();

  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  int result = virNWFilterGetUUIDString(Handle().ToNetworkFilter(), uuid);
  if (result == -1) {
    SetVirError(virGetLastError());
    delete[] uuid;
    return;
  }

  data_ = result;
  delete[] uuid;
}

NLV_WORKER_METHOD_NO_ARGS(NetworkFilter, Undefine)
NLV_WORKER_EXECUTE(NetworkFilter, Undefine)
{
  NLV_WORKER_ASSERT_INTERFACE();
  int result = virNWFilterUndefine(Handle().ToNetworkFilter());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(NetworkFilter, ToXml)
NLV_WORKER_EXECUTE(NetworkFilter, ToXml)
{
  NLV_WORKER_ASSERT_INTERFACE();

  unsigned int flags = 0;
  char *result = virNWFilterGetXMLDesc(Handle().ToNetworkFilter(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

}   // namespce NodeLibvirt

