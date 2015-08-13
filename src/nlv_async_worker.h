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
  using NLVAsyncWorker<HandleType>::callback;
public:
  explicit NLVPrimitiveReturnWorker(NanCallback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
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
protected:
  using NLVAsyncWorker<HandleType>::callback;
public:
  explicit NLVListReturnWorker(NanCallback *callback, HandleType handle)
    : NLVAsyncWorker<HandleType>(callback, handle) {}

protected:
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


#endif  // NLV_ASYNC_WORKER_H
