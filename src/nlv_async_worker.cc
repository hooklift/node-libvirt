#include "error.h"
#include "nlv_async_worker.h"

using namespace NodeLibvirt;

NLVAsyncWorkerBase::NLVAsyncWorkerBase(NanCallback *callback)
    : NanAsyncWorker(callback),
      virError_(NULL)
{
}

virErrorPtr NLVAsyncWorkerBase::VirError() const
{
  return virError_;
}

void NLVAsyncWorkerBase::SetVirError(virErrorPtr error)
{
  virError_ = error;
}

void NLVAsyncWorkerBase::WorkComplete()
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

void NLVAsyncWorkerBase::HandleErrorCallback()
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
