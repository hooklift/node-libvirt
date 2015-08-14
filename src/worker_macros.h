#ifndef WORKER_MACROS_H
#define WORKER_MACROS_H

// UTIL
#define NLV_CATNX(A, B) A ## B
#define NLV_CAT(A, B) NLV_CATNX(A, B)

// ASSERTIONS
#define NLV_WORKER_ASSERT_HANDLE(Type) \
  if (Handle() == NULL) { \
    SetErrorMessage("invalid "#Type); \
    return; \
  }

#define NLV_WORKER_ASSERT_CONNECTION2() \
  NLV_WORKER_ASSERT_HANDLE("connection")

#define NLV_WORKER_ASSERT_INTERFACE2() \
  NLV_WORKER_ASSERT_HANDLE("interface")

#define NLV_WORKER_ASSERT_NETWORK2() \
  NLV_WORKER_ASSERT_HANDLE("network")

#define NLV_WORKER_ASSERT_NODEDEVICE2() \
  NLV_WORKER_ASSERT_HANDLE("node device")

#define NLV_WORKER_ASSERT_SECRET2() \
  NLV_WORKER_ASSERT_HANDLE("secret")

#define NLV_WORKER_ASSERT_STORAGEPOOL2() \
  NLV_WORKER_ASSERT_HANDLE("storage pool")

#define NLV_WORKER_ASSERT_STORAGEVOLUME2() \
  NLV_WORKER_ASSERT_HANDLE("storage volume")

#define NLV_WORKER_ASSERT_DOMAIN2() \
  NLV_WORKER_ASSERT_HANDLE("domain")







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

#define NLV_WORKER_ASSERT_NODEDEVICE() \
  if (Handle().ToNodeDevice() == NULL) { \
    SetErrorMessage("invalid node device");  \
    return; \
  }

#define NLV_WORKER_ASSERT_SECRET() \
  if (Handle().ToSecret() == NULL) { \
    SetErrorMessage("invalid secret");  \
    return; \
  }

#define NLV_WORKER_ASSERT_DOMAIN() \
  if (Handle().ToDomain() == NULL) { \
    SetErrorMessage("invalid domain");  \
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

#define NLV_WORKER_METHOD_DEFINE(Class) \
  NAN_METHOD(Class::Define) { \
    NanScope(); \
    if (args.Length() < 2 ||  \
      (!args[0]->IsString() && !args[1]->IsFunction())) { \
      NanThrowTypeError("You must specify a string and callback");  \
      NanReturnUndefined(); \
    } \
    if (!NanHasInstance(Hypervisor::constructor_template, args.This())) { \
      NanThrowTypeError("You must specify a Hypervisor instance");  \
      NanReturnUndefined(); \
    } \
    Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This()); \
    std::string xmlData(*NanUtf8String(args[0]->ToString())); \
    NanCallback *callback = new NanCallback(args[1].As<Function>());  \
    NanAsyncQueueWorker(new DefineWorker(callback, hv->handle_, xmlData));  \
    NanReturnUndefined(); \
  }

#define NLV_WORKER_METHOD_CREATE(Class) \
  NAN_METHOD(Class::Create) { \
    NanScope(); \
    if (args.Length() < 2 ||  \
        (!args[0]->IsString() && !args[1]->IsFunction())) { \
      NanThrowTypeError("You must specify a string and callback");  \
      NanReturnUndefined(); \
    } \
    if (!NanHasInstance(Hypervisor::constructor_template, args.This())) { \
      NanThrowTypeError("You must specify a Hypervisor instance");  \
      NanReturnUndefined(); \
    } \
    Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This()); \
    std::string xmlData(*NanUtf8String(args[0]->ToString())); \
    NanCallback *callback = new NanCallback(args[1].As<Function>());  \
    NanAsyncQueueWorker(new CreateWorker(callback, hv->handle_, xmlData));  \
    NanReturnUndefined(); \
  }



// WORKER DEFINITIONS
#define NLV_PRIMITIVE_RETURN_WORKER(Method, Type)  \
  class Method##Worker : public PrimitiveReturnWorker<Type> { \
  public: \
    Method##Worker(NanCallback *callback, const LibVirtHandle &handle) \
      : PrimitiveReturnWorker<Type>(callback, handle) {} \
    void Execute(); \
  };

#define NLV_LIST_RETURN_WORKER(Method, CType, V8Type)  \
  class Method##Worker : public ListReturnWorker<CType, V8Type> { \
  public: \
    Method##Worker(NanCallback *callback, const LibVirtHandle &handle) \
      : ListReturnWorker<CType, V8Type>(callback, handle) {} \
    void Execute(); \
  };

#define NLV_LOOKUP_BY_VALUE_WORKER(Class, Method)  \
  class Method##Worker : public LookupInstanceByValueWorker<Class> { \
  public: \
    Method##Worker(NanCallback *callback, const LibVirtHandle &handle, const std::string &value) \
      : LookupInstanceByValueWorker<Class>(callback, handle, value) {} \
    void Execute(); \
  };

#define NLV_OBJECT_RETURN_WORKER(Method, ObjectType) \
  class Method##Worker : public LibVirtWorker { \
  public: \
    Method##Worker(NanCallback *callback, const LibVirtHandle &handle)  \
      : LibVirtWorker(callback, handle) {}  \
    void Execute(); \
  protected:  \
    void HandleOKCallback();  \
  private:  \
    ObjectType info_;  \
  };

#define NLV_TYPED_PARAMETER_RETURN_WORKER(Method, ParameterType)  \
  class Method##Worker : public TypedParameterReturnWorker<ParameterType> { \
  public: \
    Method##Worker(NanCallback *callback, const LibVirtHandle &handle)  \
      : TypedParameterReturnWorker<ParameterType>(callback, handle) {} \
    void Execute(); \
  };

// EXECUTE HELPERS
#define NLV_WORKER_EXECUTE(Class, Method) void Class::Method##Worker::Execute()
#define NLV_WORKER_OKCALLBACK(Class, Method) void Class::Method##Worker::HandleOKCallback()

#define NLV_INT_RETURN_EXECUTE_IMPL(Class, Method, Accessor)  \
  NLV_WORKER_EXECUTE(Class, Method) {  \
    NLV_WORKER_ASSERT_CONNECTION(); \
    int result = Accessor(Handle().ToConnection());  \
    if (result == -1) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = result;  \
  }

#define NLV_BOOL_RETURN_EXECUTE_IMPL(Class, Method, Accessor)  \
  NLV_WORKER_EXECUTE(Class, Method) {  \
    NLV_WORKER_ASSERT_CONNECTION(); \
    int result = Accessor(Handle().ToConnection());  \
    if (result == -1) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = static_cast<bool>(result);  \
  }

#define NLV_BOOL_RETURN_EXECUTE_IMPL2(Class, Method, Accessor)  \
  NLV_WORKER_EXECUTE(Class, Method) {  \
    NLV_WORKER_ASSERT_CONNECTION2(); \
    int result = Accessor(Handle());  \
    if (result == -1) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = static_cast<bool>(result);  \
  }

#define NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Class, Method, Accessor) \
  NLV_WORKER_EXECUTE(Class, Method) { \
    lookupHandle_ = Accessor(Handle().ToConnection(), value_.c_str());  \
    if (lookupHandle_.NLV_CAT(To, Class)() == NULL) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
  }

#define NLV_SP_LOOKUP_BY_VALUE_EXECUTE_IMPL(Class, Method, Accessor) \
  NLV_WORKER_EXECUTE(Class, Method) { \
    lookupHandle_ = Accessor(Handle().ToStoragePool(), value_.c_str());  \
    if (lookupHandle_.NLV_CAT(To, Class)() == NULL) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
  }


#endif  // WORKER_MACROS_H
