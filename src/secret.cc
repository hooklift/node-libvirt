// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "error.h"
#include "hypervisor.h"
#include "secret.h"

namespace NLV {

Nan::Persistent<Function> Secret::constructor;
void Secret::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>();
  t->SetClassName(Nan::New("Secret").ToLocalChecked());
  t->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(t, "undefine",      Undefine);
  Nan::SetPrototypeMethod(t, "getUUID",       GetUUID);
  Nan::SetPrototypeMethod(t, "getValue",      GetValue);
  Nan::SetPrototypeMethod(t, "setValue",      SetValue);
  Nan::SetPrototypeMethod(t, "getUsageId",    GetUsageId);
  Nan::SetPrototypeMethod(t, "getUsageType",  GetUsageType);
  Nan::SetPrototypeMethod(t, "toXml",         ToXml);

  constructor.Reset(t->GetFunction());
  exports->Set(Nan::New("Secret").ToLocalChecked(), t->GetFunction());
}

Secret::Secret(virSecretPtr handle) : NLVObject(handle) {}
Local<Object> Secret::NewInstance(virSecretPtr handle)
{
  Nan::EscapableHandleScope scope;
  Local<Function> ctor = Nan::New<Function>(constructor);
  Local<Object> object = ctor->NewInstance();

  Secret *secret = new Secret(handle);
  secret->Wrap(object);
  return scope.Escape(object);
}

NLV_WORKER_METHOD_DEFINE(Secret)
NLV_WORKER_EXECUTE(Secret, Define)
{
  NLV_WORKER_ASSERT_PARENT_HANDLE();
  unsigned int flags = 0;
  lookupHandle_ = virSecretDefineXML(parent_->handle_, value_.c_str(), flags);
  if (lookupHandle_ == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(Secret, Undefine)
NLV_WORKER_EXECUTE(Secret, Undefine)
{
  NLV_WORKER_ASSERT_SECRET();
  int result = virSecretUndefine(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(Secret::LookupByUsage)
{
  Nan::HandleScope scope;
  if (info.Length() < 3) {
    Nan::ThrowTypeError("You must specify type, xml and callback");
    return;
  }

  if (!info[0]->IsInt32()) {
    Nan::ThrowTypeError("You must specify a number as first argument");
    return;
  }

  if (!info[1]->IsString()) {
    Nan::ThrowTypeError("You must specify a string as second argument");
    return;
  }

  if (!info[2]->IsFunction()) {
    Nan::ThrowTypeError("You must specify a callback as the third argument");
    return;
  }

  Local<Object> hyp_obj = info.This();
  if (!Nan::New(Hypervisor::constructor_template)->HasInstance(hyp_obj)) {
    Nan::ThrowTypeError("You must specify a Hypervisor instance");
    return;
  }

  Hypervisor *hv = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  int usageType = info[0]->Int32Value();
  std::string usageId(*Nan::Utf8String(info[1]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());
  Nan::AsyncQueueWorker(new LookupByUsageWorker(callback, hv, usageId, usageType));
  return;
}

NLV_WORKER_EXECUTE(Secret, LookupByUsage)
{
  NLV_WORKER_ASSERT_PARENT_HANDLE();
  lookupHandle_ = virSecretLookupByUsage(parent_->handle_, usageType_, value_.c_str());
  if (lookupHandle_ == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Secret, LookupByUUID, virSecretLookupByUUIDString)
NAN_METHOD(Secret::LookupByUUID)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a valid uuid and callback.");
    return;
  }

  if (!Nan::New(Hypervisor::constructor_template)->HasInstance(info.This())) {
    Nan::ThrowTypeError("You must specify a Hypervisor instance");
    return;
  }

  Hypervisor *hv = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  std::string uuid(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Nan::AsyncQueueWorker(new LookupByUUIDWorker(callback, hv, uuid));
  return;
}

NLV_WORKER_METHOD_NO_ARGS(Secret, GetUUID)
NLV_WORKER_EXECUTE(Secret, GetUUID)
{
  NLV_WORKER_ASSERT_SECRET();
  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  int result = virSecretGetUUIDString(Handle(), uuid);
  if (result == -1) {
    SetVirError(virGetLastError());
    delete[] uuid;
    return;
  }

  data_ = uuid;
  delete[] uuid;
}

NLV_WORKER_METHOD_NO_ARGS(Secret, GetUsageId)
NLV_WORKER_EXECUTE(Secret, GetUsageId)
{
  NLV_WORKER_ASSERT_SECRET();
  const char *result = virSecretGetUsageID(Handle());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Secret, GetUsageType)
NLV_WORKER_EXECUTE(Secret, GetUsageType)
{
  NLV_WORKER_ASSERT_SECRET();
  // int usage_type = VIR_SECRET_USAGE_TYPE_NONE;
  int result = virSecretGetUsageType(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Secret, GetValue)
NLV_WORKER_EXECUTE(Secret, GetValue)
{
  NLV_WORKER_ASSERT_SECRET();
  size_t size;
  unsigned int flags = 0;
  unsigned char *result = virSecretGetValue(Handle(), &size, flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = std::string(reinterpret_cast<char *>(result), size);
  memset(result, 0, size);
  free(result);
}

NAN_METHOD(Secret::SetValue)
{
  Nan::HandleScope scope;
  if (info.Length() < 2 ||
      (!info[0]->IsString() && !info[1]->IsFunction())) {
    Nan::ThrowTypeError("You must specify a value and callback");
    return;
  }

  std::string value(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Secret *secret = Nan::ObjectWrap::Unwrap<Secret>(info.This());
  Nan::AsyncQueueWorker(new SetValueWorker(callback, secret->handle_, value));
  return;
}

NLV_WORKER_EXECUTE(Secret, SetValue)
{
  NLV_WORKER_ASSERT_SECRET();
  unsigned int flags = 0;
  int result = virSecretSetValue(Handle(),
      reinterpret_cast<const unsigned char *>(value_.c_str()), sizeof(value_.c_str()), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Secret, ToXml)
NLV_WORKER_EXECUTE(Secret, ToXml)
{
  NLV_WORKER_ASSERT_SECRET();
  unsigned int flags = 0;
  char *result = virSecretGetXMLDesc(Handle(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

} // namespace NLV
