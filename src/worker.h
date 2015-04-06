#ifndef SRC_WORKER_H_
#define SRC_WORKER_H_

#include <nan.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#include "libvirt_handle.h"

namespace NodeLibvirt {

class Hypervisor;
class LibVirtWorker : public NanAsyncWorker
{
public:
  explicit LibVirtWorker(NanCallback *callback, const LibVirtHandle &handle);
  ~LibVirtWorker();

  LibVirtHandle Handle() const;

  virErrorPtr VirError() const;
  void SetVirError(virErrorPtr error);

  virtual void WorkComplete();

protected:
  virtual void HandleErrorCallback();

private:
  LibVirtHandle handle_;
  virErrorPtr virerror_;

};

template <typename T>
class PrimitiveReturnWorker : public LibVirtWorker
{
public:
  explicit PrimitiveReturnWorker(NanCallback *callback, const LibVirtHandle &handle)
    : LibVirtWorker(callback, handle) {}

protected:
  void HandleOKCallback() {
    NanScope();

    v8::Local<v8::Value> argv[] = {
      NanNull(),
      NanNew(data_)
    };

    callback->Call(2, argv);
  }

  T data_;
};

template <typename T1, typename T2>
class ListReturnWorker : public LibVirtWorker
{
public:
  explicit ListReturnWorker(NanCallback *callback, const LibVirtHandle &handle)
    : LibVirtWorker(callback, handle) {}

protected:
  virtual void HandleOKCallback() {
    NanScope();
    v8::Local<v8::Array> result = NanNew<v8::Array>(data_.size());
    for (int i = 0; i < data_.size(); ++i)
      result->Set(NanNew<v8::Integer>(i), NanNew<T2>(data_[i]));

    v8::Local<v8::Value> argv[] = { NanNull(), result };
    callback->Call(2, argv);
  }

  std::vector<T1> data_;
};

template <typename T>
class TypedParameterReturnWorker : public LibVirtWorker
{
public:
  explicit TypedParameterReturnWorker(NanCallback *callback, const LibVirtHandle &handle)
    : LibVirtWorker(callback, handle) {}

protected:
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

template <typename T>
class LookupInstanceByValueWorker : public LibVirtWorker
{
public:
  explicit LookupInstanceByValueWorker(NanCallback *callback,
                                       const LibVirtHandle &handle,
                                       const std::string &value)
    : LibVirtWorker(callback, handle), value_(value) {}

protected:
  virtual void HandleOKCallback() {
    NanScope();
    v8::Local<v8::Value> argv[] = {
      NanNull(),
      T::NewInstance(lookupHandle_)
    };

    callback->Call(2, argv);
  }

  std::string value_;
  LibVirtHandle lookupHandle_;

};


} // namespace NodeLibvirt

#endif // SRC_WORKER_H_
