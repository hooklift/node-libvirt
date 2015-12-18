#ifndef WORKER_MACROS_H
#define WORKER_MACROS_H

// ASSERTIONS
#define NLV_WORKER_ASSERT_HANDLE(Type) \
  if (Handle() == NULL) { \
    SetErrorMessage("invalid "#Type); \
    return; \
  }

#define NLV_WORKER_ASSERT_CONNECTION() \
  NLV_WORKER_ASSERT_HANDLE("connection")

#define NLV_WORKER_ASSERT_INTERFACE() \
  NLV_WORKER_ASSERT_HANDLE("interface")

#define NLV_WORKER_ASSERT_NETWORK() \
  NLV_WORKER_ASSERT_HANDLE("network")

#define NLV_WORKER_ASSERT_NODEDEVICE() \
  NLV_WORKER_ASSERT_HANDLE("node device")

#define NLV_WORKER_ASSERT_SECRET() \
  NLV_WORKER_ASSERT_HANDLE("secret")

#define NLV_WORKER_ASSERT_STORAGEPOOL() \
  NLV_WORKER_ASSERT_HANDLE("storage pool")

#define NLV_WORKER_ASSERT_STORAGEVOLUME() \
  NLV_WORKER_ASSERT_HANDLE("storage volume")

#define NLV_WORKER_ASSERT_DOMAIN() \
  NLV_WORKER_ASSERT_HANDLE("domain")

#define NLV_WORKER_ASSERT_PARENT_HANDLE() \
  if (parent_->handle_ == NULL) { \
    SetErrorMessage("invalid parent"); \
    return; \
  }

// METHOD HELPERS
#define NLV_WORKER_METHOD_NO_ARGS(Class, Method) \
  NAN_METHOD(Class::Method) {  \
    Nan::HandleScope scope; \
    if (info.Length() == 1 && !info[0]->IsFunction()) { \
      Nan::ThrowTypeError("You must specify a function as first argument"); \
      return; \
    } \
    Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());  \
    Class *object = Nan::ObjectWrap::Unwrap<Class>(info.This()); \
    Nan::AsyncQueueWorker(new Method##Worker(callback, object->handle_));  \
    return; \
  }

#define NLV_WORKER_METHOD_DEFINE(Class) \
  NAN_METHOD(Class::Define) { \
    Nan::HandleScope scope; \
    if (info.Length() < 2 ||  \
      (!info[0]->IsString() && !info[1]->IsFunction())) { \
      Nan::ThrowTypeError("You must specify a string and callback");  \
      return; \
    } \
    if (!Nan::New(Hypervisor::constructor_template)->HasInstance(info.This())) { \
      Nan::ThrowTypeError("You must specify a Hypervisor instance");  \
      return; \
    } \
    Hypervisor *hv = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This()); \
    std::string xmlData(*Nan::Utf8String(info[0]->ToString())); \
    Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());  \
    NLV::AsyncQueueWorker(new DefineWorker(callback, hv, xmlData), info.This()); \
    return; \
  }

#define NLV_WORKER_METHOD_CREATE(Class) \
  NAN_METHOD(Class::Create) { \
    Nan::HandleScope scope; \
    if (info.Length() < 2 ||  \
        (!info[0]->IsString() && !info[1]->IsFunction())) { \
      Nan::ThrowTypeError("You must specify a string and callback");  \
      return; \
    } \
    if (!Nan::New(Hypervisor::constructor_template)->HasInstance(info.This())) { \
      Nan::ThrowTypeError("You must specify a Hypervisor instance");  \
      return; \
    } \
    Hypervisor *hv = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This()); \
    std::string xmlData(*Nan::Utf8String(info[0]->ToString())); \
    Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());  \
    NLV::AsyncQueueWorker(new CreateWorker(callback, hv, xmlData), info.This()); \
    return; \
  }

// EXECUTE HELPERS
#define NLV_WORKER_EXECUTE(Class, Method) void Class::Method##Worker::Execute()
#define NLV_WORKER_OKCALLBACK(Class, Method) void Class::Method##Worker::HandleOKCallback()

#define NLV_BOOL_RETURN_EXECUTE_IMPL(Class, Method, Accessor)  \
  NLV_WORKER_EXECUTE(Class, Method) {  \
    NLV_WORKER_ASSERT_CONNECTION(); \
    int result = Accessor(Handle());  \
    if (result == -1) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = static_cast<bool>(result);  \
  }

#endif  // WORKER_MACROS_H
