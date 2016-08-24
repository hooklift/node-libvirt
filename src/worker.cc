#include "worker.h"

using namespace NLV;

void Worker::RunAsync(const Nan::FunctionCallbackInfo<v8::Value>& info, ExecuteHandler handler) {
  Nan::TryCatch tryCatch;
  if(tryCatch.HasCaught()) {
    tryCatch.ReThrow();
    return; // don't proceed in case there were any previous errors
  }
  auto v8_callback = info[info.Length()-1];
  if(!v8_callback->IsFunction()) {
    Nan::ThrowTypeError("you must specify a function as the callback and it must the last argument");;
    return;
  }
  Nan::Callback *callback = new Nan::Callback(v8_callback.As<Function>());
  auto worker = new NLV::Worker(callback, handler);
  auto parent = info.This();
  if (!parent->IsUndefined() && !parent->IsNull()) {
    worker->SaveToPersistent("parent", parent);
  }
  Nan::AsyncQueueWorker(worker);
}
