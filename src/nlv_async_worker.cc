#include "error.h"
#include "nlv_async_worker.h"

using namespace NLV;

NLVAsyncWorkerBase::NLVAsyncWorkerBase(Nan::Callback *callback)
    : Nan::AsyncWorker(callback),
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
  Nan::HandleScope scope;
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
  Nan::HandleScope scope;
  if (virError_ != NULL) {
    v8::Local<v8::Value> argv[] = { Error::New(VirError()) };
    callback->Call(1, argv);
  } else {
    v8::Local<v8::Value> argv[] = {
      v8::Exception::Error(Nan::New<v8::String>(ErrorMessage()).ToLocalChecked())
    };
    callback->Call(1, argv);
  }
}
