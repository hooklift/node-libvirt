#include <functional>

#include "nlv_async_worker.h"

namespace NLV {
  class Worker : public NLVAsyncWorkerBase {
  public:
    typedef std::function<void(Nan::Callback*)>  OnFinishedHandler;
    
    typedef std::function<virErrorPtr(OnFinishedHandler)> SetOnFinishedHandler;
    
    typedef std::function<virErrorPtr(SetOnFinishedHandler)> ExecuteHandler;
    
    ExecuteHandler execute_handler;
    OnFinishedHandler on_finished_handler;
    
    explicit Worker(Nan::Callback *callback, ExecuteHandler handler)
      : NLVAsyncWorkerBase(callback), execute_handler(handler) { };
    
    void HandleOKCallback() {
      on_finished_handler(callback);
    }
    
    virtual void Execute() {
      auto error = execute_handler([=] (OnFinishedHandler handler) {
        on_finished_handler = handler;
        return nullptr;
      });
      if(error) {
        SetVirError(error);
      }
    }
    
    // TODO: make it a template so that it can accept arbitrary number of arguments
    // of objects to make persistent for the duration of the worker run
    static void Queue(v8::Local<v8::Value> v8_callback, ExecuteHandler handler);
  };
  
  template<class T>
  Worker::OnFinishedHandler PrimitiveReturnHandler(T val) {
    return [=](Nan::Callback* callback) {
      Nan::HandleScope scope;
      v8::Local<v8::Value> argv[] = { Nan::Null(), Nan::New(val) };
      callback->Call(2, argv);
    };
  }
};

