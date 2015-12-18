#ifndef NLV_ASYNC_WORKER_H
#define NLV_ASYNC_WORKER_H

#include <nan.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#include <vector>

#include "nlv_object.h"

/**
 * Base class for all workers in node-libvirt
 */
class NLVAsyncWorkerBase : public Nan::AsyncWorker
{
public:
  explicit NLVAsyncWorkerBase(Nan::Callback *callback);

  virErrorPtr VirError() const;
  void SetVirError(virErrorPtr error);

  virtual void WorkComplete();

protected:
  virtual void HandleErrorCallback();

private:
  virErrorPtr virError_;

};

/**
 * Base class for most NLV async workers
 */
template <typename T>
class NLVAsyncWorker : public NLVAsyncWorkerBase
{
public:
  explicit NLVAsyncWorker(Nan::Callback *callback, T handle)
    : NLVAsyncWorkerBase(callback), handle_(handle) {}

  T Handle() const { return handle_; }

private:
  T handle_;

};

/**
 * Worker that returns a primitive to javascript
 */
#define NLV_PRIMITIVE_RETURN_WORKER(Method, HandleType, PrimitiveType)  \
  class Method##Worker : public NLVPrimitiveReturnWorker<HandleType, PrimitiveType> { \
  public: \
    Method##Worker(Nan::Callback *callback, HandleType handle) \
      : NLVPrimitiveReturnWorker<HandleType, PrimitiveType>(callback, handle) {} \
    void Execute(); \
  };

template <typename HandleType, typename PrimitiveType>
class NLVPrimitiveReturnWorker : public NLVAsyncWorker<HandleType>
{
public:
  explicit NLVPrimitiveReturnWorker(Nan::Callback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
  using NLVAsyncWorker<HandleType>::callback;

  void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = { Nan::Null(), Nan::New(data_) };
    callback->Call(2, argv);
  }

  PrimitiveType data_;
};

/**
 * Worker that returns a primitive to javascript
 */
#define NLV_STRING_RETURN_WORKER(Method, HandleType, PrimitiveType)  \
  class Method##Worker : public NLVStringReturnWorker<HandleType, PrimitiveType> { \
  public: \
    Method##Worker(Nan::Callback *callback, HandleType handle) \
      : NLVStringReturnWorker<HandleType, PrimitiveType>(callback, handle) {} \
    void Execute(); \
  };

template <typename HandleType, typename PrimitiveType>
class NLVStringReturnWorker : public NLVAsyncWorker<HandleType>
{
public:
  explicit NLVStringReturnWorker(Nan::Callback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
  using NLVAsyncWorker<HandleType>::callback;

  void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = { Nan::Null(), Nan::New(data_).ToLocalChecked() };
    callback->Call(2, argv);
  }

  PrimitiveType data_;
};


/**
 * Worker that returns lists
 */
#define NLV_LIST_RETURN_WORKER(Method, HandleType, CType, V8Type)  \
  class Method##Worker : public NLVListReturnWorker<HandleType, CType, V8Type> { \
  public: \
    Method##Worker(Nan::Callback *callback, HandleType handle) \
      : NLVListReturnWorker<HandleType, CType, V8Type>(callback, handle) {} \
    void Execute(); \
  };

template <typename HandleType, typename CType, typename V8Type>
class NLVListReturnWorker : public NLVAsyncWorker<HandleType>
{
public:
  explicit NLVListReturnWorker(Nan::Callback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
  using NLVAsyncWorker<HandleType>::callback;

  virtual void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Array> result = Nan::New<v8::Array>(data_.size());
    for (unsigned int i = 0; i < data_.size(); ++i)
      result->Set(i, Nan::New<V8Type>(data_[i]));

    v8::Local<v8::Value> argv[] = { Nan::Null(), result };
    callback->Call(2, argv);
  }

  std::vector<CType> data_;
};

/**
 * Worker that returns lists
 */
#define NLV_STRING_LIST_RETURN_WORKER(Method, HandleType, CType, V8Type)  \
  class Method##Worker : public NLVStringListReturnWorker<HandleType, CType, V8Type> { \
  public: \
    Method##Worker(Nan::Callback *callback, HandleType handle) \
      : NLVStringListReturnWorker<HandleType, CType, V8Type>(callback, handle) {} \
    void Execute(); \
  };

template <typename HandleType, typename CType, typename V8Type>
class NLVStringListReturnWorker : public NLVAsyncWorker<HandleType>
{
public:
  explicit NLVStringListReturnWorker(Nan::Callback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
  using NLVAsyncWorker<HandleType>::callback;

  virtual void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Array> result = Nan::New<v8::Array>(data_.size());
    for (unsigned int i = 0; i < data_.size(); ++i)
      result->Set(i, Nan::New<V8Type>(data_[i]).ToLocalChecked());

    v8::Local<v8::Value> argv[] = { Nan::Null(), result };
    callback->Call(2, argv);
  }

  std::vector<CType> data_;
};

/**
 * Worker that returns objects
 */
#define NLV_OBJECT_RETURN_WORKER(Method, HandleType, ObjectType) \
  class Method##Worker : public NLVAsyncWorker<HandleType> { \
  public: \
    Method##Worker(Nan::Callback *callback, HandleType handle)  \
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
#define NLV_TYPED_PARAMETER_RETURN_WORKER(Method, HandleType, ParameterType)  \
  class Method##Worker : public NLVTypedParameterReturnWorker<HandleType, ParameterType> { \
  public: \
    Method##Worker(Nan::Callback *callback, HandleType handle)  \
      : NLVTypedParameterReturnWorker<HandleType, ParameterType>(callback, handle) {} \
    void Execute(); \
  };

template <typename HandleType, typename T>
class NLVTypedParameterReturnWorker : public NLVAsyncWorker<HandleType>
{
public:
  explicit NLVTypedParameterReturnWorker(Nan::Callback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
  using NLVAsyncWorker<HandleType>::callback;
  virtual void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    typename std::vector<T>::const_iterator it;
    for (it = params_.begin(); it != params_.end(); ++it) {
      v8::Local<v8::Value> value;
      switch((*it).type) {
      case VIR_TYPED_PARAM_INT:
        value = Nan::New<v8::Integer>((*it).value.i);
        break;
      case VIR_TYPED_PARAM_UINT:
        value = Nan::New<v8::Integer>((*it).value.ui);
        break;
      case VIR_TYPED_PARAM_LLONG:
        value = Nan::New<v8::Number>((*it).value.l);
        break;
      case VIR_TYPED_PARAM_ULLONG:
        value = Nan::New<v8::Number>((*it).value.ul);
        break;
      case VIR_TYPED_PARAM_DOUBLE:
        value = Nan::New<v8::Number>((*it).value.d);
        break;
      case VIR_TYPED_PARAM_BOOLEAN:
        value = Nan::New<v8::Boolean>((*it).value.b);
        break;
      default:
        value = Nan::Null();
      }

      result->Set(Nan::New((*it).field).ToLocalChecked(), value);
    }

    v8::Local<v8::Value> argv[] = { Nan::Null(), result };
    callback->Call(2, argv);
  };

  std::vector<T> params_;
};

/**
 * Worker that returns and looked up instance
 */
#define NLV_LOOKUP_BY_VALUE_WORKER(Method, InstanceClass, ParentClass, InstanceType)  \
  class Method##Worker : public NLVLookupInstanceByValueWorker<InstanceClass, ParentClass, InstanceType> { \
  public: \
    Method##Worker(Nan::Callback *callback, ParentClass *parent, const std::string &value) \
      : NLVLookupInstanceByValueWorker<InstanceClass, ParentClass, InstanceType>(callback, parent, value) {} \
    void Execute(); \
  };

#define NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Class, Method, Accessor) \
  NLV_WORKER_EXECUTE(Class, Method) { \
    lookupHandle_ = Accessor(parent_->handle_, value_.c_str());  \
    if (lookupHandle_ == NULL) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
  }

template <typename InstanceClass, typename ParentClass, typename InstanceHandleType>
class NLVLookupInstanceByValueWorker : public NLVAsyncWorkerBase
{
public:
  explicit NLVLookupInstanceByValueWorker(Nan::Callback *callback,
                                          ParentClass *parent,
                                          const std::string &value)
    : NLVAsyncWorkerBase(callback), parent_(parent), value_(value), lookupHandle_(NULL) {}

protected:
  using NLVAsyncWorkerBase::callback;
  virtual void HandleOKCallback() {
    Nan::HandleScope scope;
    Local<Object> childObject = InstanceClass::NewInstance(lookupHandle_);
    Local<Value> parentObject = GetFromPersistent("parent");
    if (parentObject->IsObject()) {
      childObject->Set(Nan::New("_parent").ToLocalChecked(), parentObject);
    }

    InstanceClass *child = Nan::ObjectWrap::Unwrap<InstanceClass>(childObject);
    NLVObjectBasePtr *childPtr = new NLVObjectBasePtr(child);
    child->SetParentReference(childPtr);
    parent_->children_.push_back(childPtr);
    v8::Local<v8::Value> argv[] = { Nan::Null(), childObject };
    callback->Call(2, argv);
  }

  ParentClass *parent_;
  std::string value_;
  InstanceHandleType lookupHandle_;

};

#endif  // NLV_ASYNC_WORKER_H
