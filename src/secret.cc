// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include <string.h>

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

#include "secret.h"

namespace NodeLibvirt {

Persistent<FunctionTemplate> Secret::constructor_template;
void Secret::Initialize()
{
  Local<FunctionTemplate> t = FunctionTemplate::New();
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "undefine", Undefine);
  NODE_SET_PROTOTYPE_METHOD(t, "getUUID", GetUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "getValue", GetValue);
  NODE_SET_PROTOTYPE_METHOD(t, "setValue", SetValue);
  NODE_SET_PROTOTYPE_METHOD(t, "getUsageId", GetUsageId);
  NODE_SET_PROTOTYPE_METHOD(t, "getUsageType", GetUsageType);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml", ToXml);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(String::NewSymbol("Secret"));
}

NAN_METHOD(Secret::Define)
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
  Secret *secret = new Secret();
  secret->secret_ = virSecretDefineXML(hypervisor->Connection(), (const char *) *xml, flags);
  if (secret->secret_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> secret_obj = constructor_template->GetFunction()->NewInstance();
  secret->Wrap(secret_obj);
  NanReturnValue(secret_obj);
}

NAN_METHOD(Secret::Undefine)
{
  NanScope();

  int ret = -1;
  Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
  ret = virSecretUndefine(secret->secret_);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  if (secret->secret_ != NULL) {
    virSecretFree(secret->secret_);
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Secret::LookupByUsage)
{
  NanScope();

  int usage_type = VIR_SECRET_USAGE_TYPE_NONE;
  if (args.Length() < 2) {
    NanThrowTypeError("You must specify two arguments to call this function");
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

  Local<Object> hyp_obj = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  usage_type = args[0]->Int32Value();
  String::Utf8Value usage_id(args[1]->ToString());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
  Secret *secret = new Secret();
  secret->secret_ = virSecretLookupByUsage(hypervisor->Connection(), usage_type, (const char *) *usage_id);
  if (secret->secret_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> secret_obj = constructor_template->GetFunction()->NewInstance();
  secret->Wrap(secret_obj);
  NanReturnValue(secret_obj);
}

NAN_METHOD(Secret::LookupByUUID)
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
  Secret *secret = new Secret();
  secret->secret_ = virSecretLookupByUUIDString(hypervisor->Connection(), (const char *) *uuid);
  if (secret->secret_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Object> secret_obj = constructor_template->GetFunction()->NewInstance();
  secret->Wrap(secret_obj);
  NanReturnValue(secret_obj);
}

NAN_METHOD(Secret::GetUUID)
{
  NanScope();

  int ret = -1;
  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
  ret = virSecretGetUUIDString(secret->secret_, uuid);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    delete[] uuid;
    NanReturnUndefined();
  }

  Local<String> uuid_str = NanNew(uuid);
  delete[] uuid;
  NanReturnValue(uuid_str);
}

NAN_METHOD(Secret::GetUsageId)
{
  NanScope();

  const char *usage_id = NULL;
  Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
  usage_id = virSecretGetUsageID(secret->secret_);
  if (usage_id == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(usage_id));
}

NAN_METHOD(Secret::GetUsageType)
{
  NanScope();

  int usage_type = VIR_SECRET_USAGE_TYPE_NONE;
  Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
  usage_type = virSecretGetUsageType(secret->secret_);
  if (usage_type == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew(usage_type));
}

NAN_METHOD(Secret::GetValue)
{
  NanScope();

  size_t size;
  unsigned int flags = 0;
  unsigned char* value_ = NULL;

  Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
  value_ = virSecretGetValue(secret->secret_, &size, flags);
  if (value_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<String> value = NanNew(std::string((const char *)value_, size));
  memset(value_, 0, size);
  free(value_);

  NanReturnValue(value);
}

NAN_METHOD(Secret::SetValue)
{
  NanScope();

  const char *value = NULL;
  unsigned int flags = 0;
  int ret = -1;
  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a valid secret value");
    NanReturnUndefined();
  }

  value = *NanUtf8String(args[0]->ToString());
  Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
  ret = virSecretSetValue(secret->secret_, (const unsigned char *)value, sizeof(*value), flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

NAN_METHOD(Secret::ToXml)
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

  Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
  xml_ = virSecretGetXMLDesc(secret->secret_, flags);
  if (xml_ == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<String> xml = NanNew(xml_);
  free(xml_);
  NanReturnValue(xml);
}

}
