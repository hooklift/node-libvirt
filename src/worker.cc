#include "worker.h"

using namespace NLV;

void Worker::Queue(v8::Local<v8::Value> v8_callback, ExecuteHandler handler, v8::Local<v8::Object> parent) {
  if(!v8_callback->IsFunction()) {
    Nan::ThrowTypeError("you must specify a function as the callback");;
    return;
  }
  Nan::Callback *callback = new Nan::Callback(v8_callback.As<Function>());
  auto worker = new NLV::Worker(callback, handler);
  if (!parent->IsUndefined() && !parent->IsNull()) {
    worker->SaveToPersistent("parent", parent);
  }
  Nan::AsyncQueueWorker(worker);
}
