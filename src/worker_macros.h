#ifndef WORKER_MACROS_H
#define WORKER_MACROS_H

// UTIL
#define NLV_CATNX(A, B) A ## B
#define NLV_CAT(A, B) NLV_CATNX(A, B)

// ASSERTIONS
#define NLV_WORKER_ASSERT_CONNECTION() \
  if (Handle().ToConnection() == NULL) { \
    SetErrorMessage("invalid connection");  \
    return; \
  }

#define NLV_WORKER_ASSERT_INTERFACE() \
  if (Handle().ToInterface() == NULL) { \
    SetErrorMessage("invalid interface");  \
    return; \
  }

#define NLV_WORKER_ASSERT_NETWORK() \
  if (Handle().ToNetwork() == NULL) { \
    SetErrorMessage("invalid network");  \
    return; \
  }

#define NLV_WORKER_ASSERT_STORAGEPOOL() \
  if (Handle().ToStoragePool() == NULL) { \
    SetErrorMessage("invalid storage pool");  \
    return; \
  }

#define NLV_WORKER_ASSERT_STORAGEVOLUME() \
  if (Handle().ToStorageVolume() == NULL) { \
    SetErrorMessage("invalid storage volume");  \
    return; \
  }


// METHOD HELPERS
#define NLV_WORKER_METHOD_NO_ARGS(Class, Method) \
NAN_METHOD(Class::Method) {  \
  NanScope(); \
  if (args.Length() == 1 && !args[0]->IsFunction()) { \
    NanThrowTypeError("You must specify a function as first argument"); \
    NanReturnUndefined(); \
  } \
  NanCallback *callback = new NanCallback(args[0].As<Function>());  \
  Class *object = ObjectWrap::Unwrap<Class>(args.This()); \
  NanAsyncQueueWorker(new Method##Worker(callback, object->handle_));  \
  NanReturnUndefined(); \
}

// WORKER DEFINITIONS
#define NLV_PRIMITIVE_RETURN_WORKER(Method, HandleType, Type)  \
  class Method##Worker : public PrimitiveReturnWorker<Type> { \
  public: \
    Method##Worker(NanCallback *callback, HandleType handle) \
      : PrimitiveReturnWorker<Type>(callback, handle) {} \
    void Execute(); \
  };

#define NLV_LOOKUP_BY_VALUE_WORKER(Class, Method)  \
  class Method##Worker : public LookupInstanceByValueWorker<Class> { \
  public: \
    Method##Worker(NanCallback *callback, const LibVirtHandle &handle, const std::string &value) \
      : LookupInstanceByValueWorker<Class>(callback, handle, value) {} \
    void Execute(); \
  };

// EXECUTE HELPERS
#define NLV_INT_RETURN_EXECUTE(Class, Method, Accessor)  \
  void Class::Method##Worker::Execute() {  \
    HYPERVISOR_ASSERT_CONNECTION(); \
    int result = Accessor(Handle().ToConnection());  \
    if (result == -1) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = result;  \
  }

#define NLV_BOOL_RETURN_EXECUTE(Class, Method, Accessor)  \
  void Class::Method##Worker::Execute() {  \
    HYPERVISOR_ASSERT_CONNECTION(); \
    int result = Accessor(Handle().ToConnection());  \
    if (result == -1) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = static_cast<bool>(result);  \
  }

#define NLV_LOOKUP_BY_VALUE_EXECUTE(Class, Method, Accessor) \
  void Class::Method##Worker::Execute() { \
    lookupHandle_ = Accessor(Handle().ToConnection(), value_.c_str());  \
    if (lookupHandle_.NLV_CAT(To, Class)() == NULL) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
  }

#endif  // WORKER_MACROS_H
