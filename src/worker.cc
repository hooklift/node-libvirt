#include "worker.h"

using namespace NLV;

void Worker::Queue(v8::Handle<v8::Value> v8_callback, ExecuteHandler handler) {
  if(!v8_callback->IsFunction()) {
    Nan::ThrowTypeError("you must specify a function as the callback");;
    return;
  }
  Nan::Callback *callback = new Nan::Callback(v8_callback.As<Function>());
  auto worker = new NLV::Worker(callback, handler);
  Nan::AsyncQueueWorker(worker);
}
