// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

#include "error.h"
#include "hypervisor.h"
#include "network_filter.h"

namespace NLV {

Nan::Persistent<Function> NetworkFilter::constructor;
void NetworkFilter::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>();
  t->SetClassName(Nan::New("NetworkFilter").ToLocalChecked());
  t->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(t, "getName", NetworkFilter::GetName);
  Nan::SetPrototypeMethod(t, "getUUID", NetworkFilter::GetUUID);
  Nan::SetPrototypeMethod(t, "undefine", NetworkFilter::Undefine);
  Nan::SetPrototypeMethod(t, "toXml", NetworkFilter::ToXml);

  constructor.Reset(t->GetFunction());
  exports->Set(Nan::New("NetworkFilter").ToLocalChecked(), t->GetFunction());
}

NetworkFilter::NetworkFilter(virNWFilterPtr handle) : NLVObject(handle) {}
Local<Object> NetworkFilter::NewInstance(virNWFilterPtr handle)
{
  Nan::EscapableHandleScope scope;
  Local<Function> ctor = Nan::New<Function>(constructor);
  Local<Object> object = ctor->NewInstance();

  NetworkFilter *filter = new NetworkFilter(handle);
  filter->Wrap(object);
  return scope.Escape(object);
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(NetworkFilter, LookupByName, virNWFilterLookupByName)
NAN_METHOD(NetworkFilter::LookupByName)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a valid network filter name and callback.");
    return;
  }

  Local<Object> object = info.This();
  if (!Nan::New(Hypervisor::constructor_template)->HasInstance(object)) {
    Nan::ThrowTypeError("You must specify a Hypervisor instance");
    return;
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(object);
  std::string name(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Nan::AsyncQueueWorker(new LookupByNameWorker(callback, hv, name));
  return;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(NetworkFilter, LookupByUUID, virNWFilterLookupByUUIDString)
NAN_METHOD(NetworkFilter::LookupByUUID)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a valid network filter uuid and callback.");
    return;
  }

  if (!Nan::New(Hypervisor::constructor_template)->HasInstance(info.This())) {
    Nan::ThrowTypeError("You must specify a Hypervisor instance");
    return;
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(info.This());
  std::string uuid(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Nan::AsyncQueueWorker(new LookupByUUIDWorker(callback, hv, uuid));
  return;
}

NLV_WORKER_METHOD_DEFINE(NetworkFilter)
NLV_WORKER_EXECUTE(NetworkFilter, Define)
{
  NLV_WORKER_ASSERT_PARENT_HANDLE();
  lookupHandle_ = virNWFilterDefineXML(parent_->handle_, value_.c_str());
  if (lookupHandle_ == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(NetworkFilter, GetName)
NLV_WORKER_EXECUTE(NetworkFilter, GetName)
{
  NLV_WORKER_ASSERT_INTERFACE();
  const char *result = virNWFilterGetName(Handle());
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
  int result = virNWFilterGetUUIDString(Handle(), uuid);
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
  int result = virNWFilterUndefine(Handle());
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
  char *result = virNWFilterGetXMLDesc(Handle(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

}   // namespce NLV
