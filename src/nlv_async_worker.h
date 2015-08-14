#ifndef NLV_ASYNC_WORKER_H
#define NLV_ASYNC_WORKER_H

#include <nan.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#include "error.h"

using namespace NodeLibvirt;

/**
 * Base class for all workers in node-libvirt
 */
template <typename T>
class NLVAsyncWorker : public NanAsyncWorker
{
public:
  explicit NLVAsyncWorker(NanCallback *callback, T handle)
    : NanAsyncWorker(callback), handle_(handle), virError_(NULL) {}

  T Handle() const { return handle_; }

  virErrorPtr VirError() const { return virError_; }
  void SetVirError(virErrorPtr error) { virError_ = error; }

  virtual void WorkComplete()
  {
    NanScope();

    if (virError_ == NULL && ErrorMessage() == NULL) {
      HandleOKCallback();
    } else {
      HandleErrorCallback();
    }

    delete callback;
    callback = NULL;
  }

protected:
  virtual void HandleErrorCallback()
  {
    NanScope();

    if (virError_ != NULL) {
      v8::Handle<v8::Value> argv[] = { Error::New(VirError()) };
      callback->Call(1, argv);
    } else {
      v8::Local<v8::Value> argv[] = {
        v8::Exception::Error(NanNew<v8::String>(ErrorMessage()))
      };
      callback->Call(1, argv);
    }
  }

private:
  T handle_;
  virErrorPtr virError_;

};

/**
 * Worker that returns a primitive to javascript
 */
#define NLV_PRIMITIVE_RETURN_WORKER2(Method, HandleType, PrimitiveType)  \
  class Method##Worker : public NLVPrimitiveReturnWorker<HandleType, PrimitiveType> { \
  public: \
    Method##Worker(NanCallback *callback, HandleType handle) \
      : NLVPrimitiveReturnWorker<HandleType, PrimitiveType>(callback, handle) {} \
    void Execute(); \
  };

template <typename HandleType, typename PrimitiveType>
class NLVPrimitiveReturnWorker : public NLVAsyncWorker<HandleType>
{
public:
  explicit NLVPrimitiveReturnWorker(NanCallback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
  using NLVAsyncWorker<HandleType>::callback;

  void HandleOKCallback() {
    NanScope();
    v8::Local<v8::Value> argv[] = { NanNull(), NanNew(data_) };
    callback->Call(2, argv);
  }

  PrimitiveType data_;
};

/**
 * Worker that returns lists
 */
#define NLV_LIST_RETURN_WORKER2(Method, HandleType, CType, V8Type)  \
  class Method##Worker : public NLVListReturnWorker<HandleType, CType, V8Type> { \
  public: \
    Method##Worker(NanCallback *callback, HandleType handle) \
      : NLVListReturnWorker<HandleType, CType, V8Type>(callback, handle) {} \
    void Execute(); \
  };

template <typename HandleType, typename CType, typename V8Type>
class NLVListReturnWorker : public NLVAsyncWorker<HandleType>
{
public:
  explicit NLVListReturnWorker(NanCallback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
  using NLVAsyncWorker<HandleType>::callback;

  virtual void HandleOKCallback() {
    NanScope();
    v8::Local<v8::Array> result = NanNew<v8::Array>(data_.size());
    for (unsigned int i = 0; i < data_.size(); ++i)
      result->Set(NanNew<v8::Integer>(i), NanNew<V8Type>(data_[i]));

    v8::Local<v8::Value> argv[] = { NanNull(), result };
    callback->Call(2, argv);
  }

  std::vector<CType> data_;
};

/**
 * Worker that returns objects
 */
#define NLV_OBJECT_RETURN_WORKER2(Method, HandleType, ObjectType) \
  class Method##Worker : public NLVAsyncWorker<HandleType> { \
  public: \
    Method##Worker(NanCallback *callback, HandleType handle)  \
      : NLVAsyncWorker<HandleType>(callback, handle) {}  \
    void Execute(); \
  protected:  \
    using NLVAsyncWorker<HandleType>::callback; \
    void HandleOKCallback();  \
  private:  \
    ObjectType info_;  \
  };

/**
 * Worker that returns a typed parameter object
 */
#define NLV_TYPED_PARAMETER_RETURN_WORKER2(Method, HandleType, ParameterType)  \
  class Method##Worker : public NLVTypedParameterReturnWorker<HandleType, ParameterType> { \
  public: \
    Method##Worker(NanCallback *callback, HandleType handle)  \
      : NLVTypedParameterReturnWorker<HandleType, ParameterType>(callback, handle) {} \
    void Execute(); \
  };

template <typename HandleType, typename T>
class NLVTypedParameterReturnWorker : public NLVAsyncWorker<HandleType>
{
public:
  explicit NLVTypedParameterReturnWorker(NanCallback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
  using NLVAsyncWorker<HandleType>::callback;
  virtual void HandleOKCallback() {
    NanScope();
    v8::Local<v8::Object> result = NanNew<v8::Object>();
    typename std::vector<T>::const_iterator it;
    for (it = params_.begin(); it != params_.end(); ++it) {
      v8::Local<v8::Value> value;
      switch((*it).type) {
      case VIR_TYPED_PARAM_INT:
        value = NanNew<v8::Integer>((*it).value.i);
        break;
      case VIR_TYPED_PARAM_UINT:
        value = NanNew<v8::Integer>((*it).value.ui);
        break;
      case VIR_TYPED_PARAM_LLONG:
        value = NanNew<v8::Number>((*it).value.l);
        break;
      case VIR_TYPED_PARAM_ULLONG:
        value = NanNew<v8::Number>((*it).value.ul);
        break;
      case VIR_TYPED_PARAM_DOUBLE:
        value = NanNew<v8::Number>((*it).value.d);
        break;
      case VIR_TYPED_PARAM_BOOLEAN:
        value = NanNew<v8::Boolean>((*it).value.b);
        break;
      default:
        value = NanNull();
      }

      result->Set(NanNew((*it).field), value);
    }

    v8::Local<v8::Value> argv[] = { NanNull(), result };
    callback->Call(2, argv);
  };

  std::vector<T> params_;
};

/**
 * Worker that returns and looked up instance
 */
#define NLV_LOOKUP_BY_VALUE_WORKER2(Method, InstanceClass, ParentType, InstanceType)  \
  class Method##Worker : public NLVLookupInstanceByValueWorker<InstanceClass, ParentType, InstanceType> { \
  public: \
    Method##Worker(NanCallback *callback, ParentType handle, const std::string &value) \
      : NLVLookupInstanceByValueWorker<InstanceClass, ParentType, InstanceType>(callback, handle, value) {} \
    void Execute(); \
  };

#define NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL2(Class, Method, Accessor) \
  NLV_WORKER_EXECUTE(Class, Method) { \
    lookupHandle_ = Accessor(Handle(), value_.c_str());  \
    if (lookupHandle_ == NULL) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
  }


template <typename InstanceClass, typename ParentHandleType, typename InstanceHandleType>
class NLVLookupInstanceByValueWorker : public NLVAsyncWorker<ParentHandleType>
{
public:
  explicit NLVLookupInstanceByValueWorker(NanCallback *callback,
                                          ParentHandleType handle,
                                          const std::string &value)
    : NLVAsyncWorker<ParentHandleType>(callback, handle), value_(value) {}

protected:
  using NLVAsyncWorker<ParentHandleType>::callback;
  virtual void HandleOKCallback() {
    NanScope();
    v8::Local<v8::Value> argv[] = { NanNull(), InstanceClass::NewInstance2(lookupHandle_) };
    callback->Call(2, argv);
  }

  std::string value_;
  InstanceHandleType lookupHandle_;

};


#endif  // NLV_ASYNC_WORKER_H
