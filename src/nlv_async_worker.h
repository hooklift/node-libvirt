#ifndef NLV_ASYNC_WORKER_H
#define NLV_ASYNC_WORKER_H

#include <nan.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#include "error.h"

using namespace NodeLibvirt;

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

#endif  // NLV_ASYNC_WORKER_H
