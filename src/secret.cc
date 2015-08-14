// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "error.h"
#include "hypervisor.h"
#include "secret.h"

namespace NodeLibvirt {

Persistent<Function> Secret::constructor;
void Secret::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = NanNew<FunctionTemplate>();
  t->SetClassName(NanNew("Secret"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "undefine",      Undefine);
  NODE_SET_PROTOTYPE_METHOD(t, "getUUID",       GetUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "getValue",      GetValue);
  NODE_SET_PROTOTYPE_METHOD(t, "setValue",      SetValue);
  NODE_SET_PROTOTYPE_METHOD(t, "getUsageId",    GetUsageId);
  NODE_SET_PROTOTYPE_METHOD(t, "getUsageType",  GetUsageType);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml",         ToXml);

  NanAssignPersistent(constructor, t->GetFunction());
  exports->Set(NanNew("Secret"), t->GetFunction());
}

Local<Object> Secret::NewInstance2(virSecretPtr handle)
{
  NanEscapableScope();
  Local<Function> ctor = NanNew<Function>(constructor);
  Local<Object> object = ctor->NewInstance();

  Secret *secret = new Secret(handle);
  secret->Wrap(object);
  return NanEscapeScope(object);
}

Secret::~Secret()
{
  if (handle_ != NULL)
    virSecretFree(handle_);
  handle_ = 0;
}

NLV_WORKER_METHOD_DEFINE(Secret)
NLV_WORKER_EXECUTE(Secret, Define)
{
  NLV_WORKER_ASSERT_CONNECTION2();
  unsigned int flags = 0;
  lookupHandle_ = virSecretDefineXML(Handle(), value_.c_str(), flags);
  if (lookupHandle_ == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(Secret, Undefine)
NLV_WORKER_EXECUTE(Secret, Undefine)
{
  NLV_WORKER_ASSERT_SECRET2();
  int result = virSecretUndefine(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  // @todo: cleanup actual pointer
  // if (Handle().ToSecret() != NULL) {
  //   Handle().Clear();
  // }

  data_ = true;
}

NAN_METHOD(Secret::LookupByUsage)
{
  NanScope();
  if (args.Length() < 3) {
    NanThrowTypeError("You must specify type, xml and callback");
    NanReturnUndefined();
  }

  if (!args[0]->IsInt32()) {
    NanThrowTypeError("You must specify a number as first argument");
    NanReturnUndefined();
  }

  if (!args[1]->IsString()) {
    NanThrowTypeError("You must specify a string as second argument");
    NanReturnUndefined();
  }

  if (!args[2]->IsFunction()) {
    NanThrowTypeError("You must specify a callback as the third argument");
    NanReturnUndefined();
  }

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  int usageType = args[0]->Int32Value();
  std::string usageId(*NanUtf8String(args[1]->ToString()));
  NanCallback *callback = new NanCallback(args[2].As<Function>());
  NanAsyncQueueWorker(new LookupByUsageWorker(callback, hv->handle_, usageId, usageType));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Secret, LookupByUsage)
{
  NLV_WORKER_ASSERT_CONNECTION2();
  lookupHandle_ = virSecretLookupByUsage(Handle(), usageType_, value_.c_str());
  if (lookupHandle_ == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL2(Secret, LookupByUUID, virSecretLookupByUUIDString)
NAN_METHOD(Secret::LookupByUUID)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid uuid and callback.");
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

NLV_WORKER_METHOD_NO_ARGS(Secret, GetUUID)
NLV_WORKER_EXECUTE(Secret, GetUUID)
{
  NLV_WORKER_ASSERT_SECRET2();
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
  NLV_WORKER_ASSERT_SECRET2();
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
  NLV_WORKER_ASSERT_SECRET2();
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
  NLV_WORKER_ASSERT_SECRET2();
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
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a value and callback");
    NanReturnUndefined();
  }

  std::string value(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
  NanAsyncQueueWorker(new SetValueWorker(callback, secret->handle_, value));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Secret, SetValue)
{
  NLV_WORKER_ASSERT_SECRET2();
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
  NLV_WORKER_ASSERT_SECRET2();
  unsigned int flags = 0;
  char *result = virSecretGetXMLDesc(Handle(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

} // namespace NodeLibvirt
