#include "error.h"
#include "nlv_async_worker.h"

using namespace NLV;

NLVAsyncWorkerBase::NLVAsyncWorkerBase(Nan::Callback *callback)
    : Nan::AsyncWorker(callback),
      virError_(NULL)
{
}

NLVAsyncWorkerBase::~NLVAsyncWorkerBase()
{
  // the virError_ reference SHOULD be handed off to an Error object
  // and we should probably never be destroyed while still holding the ref...
  if (virError_ != NULL) {
    virFreeError(virError_);
  }
}

virErrorPtr NLVAsyncWorkerBase::VirError() const
{
  return virError_;
}

void NLVAsyncWorkerBase::SetVirError(virErrorPtr error)
{
  if (virError_ != NULL) {
    virFreeError(virError_);
  }
  if (error != NULL && error->level == VIR_ERR_NONE) {
      virFreeError(error);
      error = NULL;
  }
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
    v8::Local<v8::Value> argv[] = { MakeVirError() };
    // the reference to virError will be cleaned up by Error object now
    virError_ = NULL;
    callback->Call(1, argv);
  } else {
    v8::Local<v8::Value> argv[] = {
      v8::Exception::Error(Nan::New<v8::String>(ErrorMessage()).ToLocalChecked())
    };
    callback->Call(1, argv);
  }
}
