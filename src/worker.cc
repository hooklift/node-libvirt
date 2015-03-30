#include "worker.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {

LibVirtWorker::LibVirtWorker(NanCallback *callback, virConnectPtr conn)
  : NanAsyncWorker(callback), conn_(conn), virerror_(NULL)
{
}

LibVirtWorker::~LibVirtWorker()
{
}

virConnectPtr LibVirtWorker::Connection() const
{
  return conn_;
}

void LibVirtWorker::SetVirError(virErrorPtr error)
{
  virerror_ = error;
}

virErrorPtr LibVirtWorker::VirError() const
{
  return virerror_;
}

void LibVirtWorker::WorkComplete()
{
  NanScope();

  if (virerror_ == NULL && ErrorMessage() == NULL)
    HandleOKCallback();
  else
    HandleErrorCallback();
  delete callback;
  callback = NULL;
}

void LibVirtWorker::HandleErrorCallback()
{
  NanScope();

  if (virerror_ != NULL) {
    v8::Handle<v8::Value> argv[] = { Error::New(VirError()) };
    callback->Call(1, argv);
  } else {
    v8::Local<v8::Value> argv[] = {
      v8::Exception::Error(NanNew<v8::String>(ErrorMessage()))
    };
    callback->Call(1, argv);
  }
}

} // namespace NodeLibvirt
