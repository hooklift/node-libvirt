// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "error.h"
#include "hypervisor.h"
#include "secret.h"

namespace NodeLibvirt {

Persistent<FunctionTemplate> Secret::constructor_template;
void Secret::Initialize()
{
  Local<FunctionTemplate> t = FunctionTemplate::New();
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "undefine",      Undefine);
  NODE_SET_PROTOTYPE_METHOD(t, "getUUID",       GetUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "getValue",      GetValue);
  NODE_SET_PROTOTYPE_METHOD(t, "setValue",      SetValue);
  NODE_SET_PROTOTYPE_METHOD(t, "getUsageId",    GetUsageId);
  NODE_SET_PROTOTYPE_METHOD(t, "getUsageType",  GetUsageType);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml",         ToXml);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(NanNew("Secret"));
}

Local<Object> Secret::NewInstance(const LibVirtHandle &handle)
{
  NanScope();
  Secret *secret = new Secret(handle.ToSecret());
  Local<Object> object = constructor_template->GetFunction()->NewInstance();
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
  unsigned int flags = 0;
  lookupHandle_ =
    virSecretDefineXML(Handle().ToConnection(), value_.c_str(), flags);
  if (lookupHandle_.ToSecret() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(Secret, Undefine)
NLV_WORKER_EXECUTE(Secret, Undefine)
{
  NLV_WORKER_ASSERT_SECRET();

  int result = virSecretUndefine(Handle().ToSecret());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  if (Handle().ToSecret() != NULL) {
    Handle().Clear();
  }

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
  lookupHandle_ =
    virSecretLookupByUsage(Handle().ToConnection(), usageType_, value_.c_str());
  if (lookupHandle_.ToSecret() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Secret, LookupByUUID, virSecretLookupByUUIDString)
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
  NLV_WORKER_ASSERT_SECRET();

  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  int result = virSecretGetUUIDString(Handle().ToSecret(), uuid);
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

  const char *result = virSecretGetUsageID(Handle().ToSecret());
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
  int result = virSecretGetUsageType(Handle().ToSecret());
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
  unsigned char *result = virSecretGetValue(Handle().ToSecret(), &size, flags);
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
  NLV_WORKER_ASSERT_SECRET();

  unsigned int flags = 0;
  int result = virSecretSetValue(Handle().ToSecret(),
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
  char *result = virSecretGetXMLDesc(Handle().ToSecret(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

} // namespace NodeLibvirt
