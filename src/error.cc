// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "error.h"

namespace NLV {

Nan::Persistent<Function> Error::constructor;
void Error::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>();
  t->SetClassName(Nan::New("Error").ToLocalChecked());

  Local<ObjectTemplate> ot = t->InstanceTemplate();
  ot->SetInternalFieldCount(1);
  Nan::SetAccessor(ot, Nan::New("code").ToLocalChecked(), Error::Getter);
  Nan::SetAccessor(ot, Nan::New("domain").ToLocalChecked(), Error::Getter);
  Nan::SetAccessor(ot, Nan::New("message").ToLocalChecked(), Error::Getter);
  Nan::SetAccessor(ot, Nan::New("level").ToLocalChecked(), Error::Getter);
  Nan::SetAccessor(ot, Nan::New("str1").ToLocalChecked(), Error::Getter);
  Nan::SetAccessor(ot, Nan::New("str2").ToLocalChecked(), Error::Getter);
  Nan::SetAccessor(ot, Nan::New("str3").ToLocalChecked(), Error::Getter);
  Nan::SetAccessor(ot, Nan::New("int1").ToLocalChecked(), Error::Getter);
  Nan::SetAccessor(ot, Nan::New("int2").ToLocalChecked(), Error::Getter);

  constructor.Reset(t->GetFunction());

  //define constants
  //virErrorNumber
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_OK);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INTERNAL_ERROR);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_MEMORY);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_SUPPORT);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_UNKNOWN_HOST);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_CONNECT);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_CONN);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_DOMAIN);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_ARG);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_OPERATION_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_GET_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_POST_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_HTTP_ERROR);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_SEXPR_SERIAL);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_XEN);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_XEN_CALL);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_OS_TYPE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_KERNEL);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_ROOT);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_SOURCE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_TARGET);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_NAME);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_OS);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_DEVICE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_XENSTORE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_DRIVER_FULL);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_CALL_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_XML_ERROR);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_DOM_EXIST);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_OPERATION_DENIED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_OPEN_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_READ_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_PARSE_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_CONF_SYNTAX);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_WRITE_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_XML_DETAIL);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_NETWORK);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NETWORK_EXIST);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_SYSTEM_ERROR);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_RPC);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_GNUTLS_ERROR);
  NODE_DEFINE_CONSTANT(exports, VIR_WAR_NO_NETWORK);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_DOMAIN);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_NETWORK);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_MAC);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_AUTH_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_STORAGE_POOL);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_STORAGE_VOL);
  NODE_DEFINE_CONSTANT(exports, VIR_WAR_NO_STORAGE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_STORAGE_POOL);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_STORAGE_VOL);
  NODE_DEFINE_CONSTANT(exports, VIR_WAR_NO_NODE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_NODE_DEVICE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_NODE_DEVICE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_SECURITY_MODEL);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_OPERATION_INVALID);
  NODE_DEFINE_CONSTANT(exports, VIR_WAR_NO_INTERFACE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_INTERFACE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_INTERFACE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_MULTIPLE_INTERFACES);
  NODE_DEFINE_CONSTANT(exports, VIR_WAR_NO_NWFILTER);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_NWFILTER);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_NWFILTER);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_BUILD_FIREWALL);
  NODE_DEFINE_CONSTANT(exports, VIR_WAR_NO_SECRET);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_SECRET);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_SECRET);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_CONFIG_UNSUPPORTED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_OPERATION_TIMEOUT);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_MIGRATE_PERSIST_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_HOOK_SCRIPT_FAILED);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_INVALID_DOMAIN_SNAPSHOT);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NO_DOMAIN_SNAPSHOT);

#if LIBVIR_CHECK_VERSION(1,2,21)
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_ARGUMENT_UNSUPPORTED);
#endif

  //virErrorDomain
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_NONE);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_XEN);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_XEND);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_XENSTORE);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_SEXPR);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_XML);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_DOM);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_RPC);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_PROXY);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_CONF);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_QEMU);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_NET);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_TEST);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_REMOTE);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_OPENVZ);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_XENXM);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_STATS_LINUX);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_LXC);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_STORAGE);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_NETWORK);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_DOMAIN);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_UML);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_NODEDEV);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_XEN_INOTIFY);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_SECURITY);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_VBOX);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_INTERFACE);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_ONE);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_ESX);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_PHYP);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_SECRET);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_CPU);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_XENAPI);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_NWFILTER);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_HOOK);
  NODE_DEFINE_CONSTANT(exports, VIR_FROM_DOMAIN_SNAPSHOT);

  //virErrorLevel
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_NONE);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_WARNING);
  NODE_DEFINE_CONSTANT(exports, VIR_ERR_ERROR);
}

Error::Error(virErrorPtr error)
  : Nan::ObjectWrap()
{
  error_ = error;
}

Local<Value> Error::New(virErrorPtr error)
{
  Nan::EscapableHandleScope scope;

  Local<Function> ctor = Nan::New<Function>(constructor);
  Local<Object> instance = ctor->NewInstance();
  Error *err = new Error(error);
  err->Wrap(instance);
  return scope.Escape(instance);
}

NAN_GETTER(Error::Getter)
{
  Nan::HandleScope scope;

  Error *error = Nan::ObjectWrap::Unwrap<Error>(info.This());
  virErrorPtr error_ = error->error_;

  if (property->Equals(Nan::New("code").ToLocalChecked())) {
    return info.GetReturnValue().Set(Nan::New(error_->code));
  } else if (property->Equals(Nan::New("domain").ToLocalChecked())) {
    return info.GetReturnValue().Set(Nan::New(error_->domain));
  } else if (property->Equals(Nan::New("message").ToLocalChecked())) {
    return info.GetReturnValue().Set(Nan::New(error_->message).ToLocalChecked());
  } else if (property->Equals(Nan::New("level").ToLocalChecked())) {
    return info.GetReturnValue().Set(Nan::New(error_->level));
  } else if (property->Equals(Nan::New("str1").ToLocalChecked())) {
    return info.GetReturnValue().Set(Nan::New(error_->str1 != NULL ? error_->str1 : "").ToLocalChecked());
  } else if (property->Equals(Nan::New("str2").ToLocalChecked())) {
    return info.GetReturnValue().Set(Nan::New(error_->str2 != NULL ? error_->str2 : "").ToLocalChecked());
  } else if (property->Equals(Nan::New("str3").ToLocalChecked())) {
    return info.GetReturnValue().Set(Nan::New(error_->str3 != NULL ? error_->str3 : "").ToLocalChecked());
  } else if (property->Equals(Nan::New("int1").ToLocalChecked())) {
    return info.GetReturnValue().Set(Nan::New(error_->int1));
  } else if (property->Equals(Nan::New("int2").ToLocalChecked())) {
    return info.GetReturnValue().Set(Nan::New(error_->int2));
  }

  return;
}

} // namespace NLV
