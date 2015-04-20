// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "hypervisor.h"
#include "worker_macros.h"
#include "interface.h"

namespace NodeLibvirt {

Persistent<Function> Interface::constructor;
void Interface::Initialize(Handle<Object> exports)
{
  Local<FunctionTemplate> t = NanNew<FunctionTemplate>();
  t->SetClassName(NanNew("Interface"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(t, "start",         Start);
  NODE_SET_PROTOTYPE_METHOD(t, "stop",          Stop);
  NODE_SET_PROTOTYPE_METHOD(t, "getName",       GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getMacAddress", GetMacAddress);
  NODE_SET_PROTOTYPE_METHOD(t, "isActive",      IsActive);
  NODE_SET_PROTOTYPE_METHOD(t, "undefine",      Undefine);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml",         ToXml);

  NanAssignPersistent(constructor, t->GetFunction());
  exports->Set(NanNew("Interface"), t->GetFunction());

  //Constants
  //virInterfaceXMLFlags
  NODE_DEFINE_CONSTANT(exports, VIR_INTERFACE_XML_INACTIVE);
}

Local<Object> Interface::NewInstance(const LibVirtHandle &handle)
{
  NanEscapableScope();
  Local<Function> ctor = NanNew<Function>(constructor);
  Local<Object> object = ctor->NewInstance();

  Interface *interface = new Interface(handle.ToInterface());
  interface->Wrap(object);
  return NanEscapeScope(object);
}

Interface::~Interface()
{
  if (handle_ != NULL)
    virInterfaceFree(handle_);
  handle_ = 0;
}

virInterfacePtr Interface::GetInterface() const
{
  return handle_;
}

NLV_WORKER_METHOD_NO_ARGS(Interface, Start)
NLV_WORKER_EXECUTE(Interface, Start)
{
  NLV_WORKER_ASSERT_INTERFACE();

  unsigned int flags = 0;
  int result = virInterfaceCreate(Handle().ToInterface(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_NO_ARGS(Interface, Stop)
NLV_WORKER_EXECUTE(Interface, Stop)
{
  NLV_WORKER_ASSERT_INTERFACE();

  unsigned int flags = 0;
  int result = virInterfaceDestroy(Handle().ToInterface(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  if (Handle().ToInterface() != NULL) {
    Handle().Clear();
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_DEFINE(Interface)
NLV_WORKER_EXECUTE(Interface, Define)
{
  unsigned int flags = 0;
  lookupHandle_ =
    virInterfaceDefineXML(Handle().ToConnection(), value_.c_str(), flags);
  if (lookupHandle_.ToInterface() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(Interface, Undefine)
NLV_WORKER_EXECUTE(Interface, Undefine)
{
  NLV_WORKER_ASSERT_INTERFACE();
  int result = virInterfaceUndefine(Handle().ToInterface());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Interface, LookupByName, virInterfaceLookupByName)
NAN_METHOD(Interface::LookupByName)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid Interface name and callback.");
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

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Interface, LookupByMacAddress, virInterfaceLookupByMACString)
NAN_METHOD(Interface::LookupByMacAddress)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid Interface MAC address and callback.");
    NanReturnUndefined();
  }

  Local<Object> object = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, object)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *unwrapped = ObjectWrap::Unwrap<Hypervisor>(object);
  std::string uuid(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByMacAddressWorker(callback, unwrapped->handle_, uuid));
  NanReturnUndefined();
}

NLV_WORKER_METHOD_NO_ARGS(Interface, GetName)
NLV_WORKER_EXECUTE(Interface, GetName)
{
  NLV_WORKER_ASSERT_INTERFACE();

  const char *result = virInterfaceGetName(Handle().ToInterface());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Interface, GetMacAddress)
NLV_WORKER_EXECUTE(Interface, GetMacAddress)
{
  NLV_WORKER_ASSERT_INTERFACE();

  const char *result = virInterfaceGetMACString(Handle().ToInterface());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Interface, IsActive)
NLV_WORKER_EXECUTE(Interface, IsActive)
{
  NLV_WORKER_ASSERT_INTERFACE();

  int result = virInterfaceIsActive(Handle().ToInterface());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_NO_ARGS(Interface, ToXml)
NLV_WORKER_EXECUTE(Interface, ToXml)
{
  NLV_WORKER_ASSERT_INTERFACE();

  unsigned int flags = 0;
  char *result = virInterfaceGetXMLDesc(Handle().ToInterface(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = strdup(result);
  free(result);
}

}   // namespace NodeLibvirt
