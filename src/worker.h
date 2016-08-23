#include <functional>

#include "nlv_async_worker.h"

namespace NLV {
  class Worker : public NLVAsyncWorkerBase {
  public:
    typedef std::function<void(Worker*)>  OnFinishedHandler;
    
    typedef std::function<virErrorPtr(OnFinishedHandler)> SetOnFinishedHandler;
    
    typedef std::function<virErrorPtr(SetOnFinishedHandler)> ExecuteHandler;
    
    ExecuteHandler execute_handler;
    OnFinishedHandler on_finished_handler;
    
    explicit Worker(Nan::Callback *callback, ExecuteHandler handler)
      : NLVAsyncWorkerBase(callback), execute_handler(handler) { };
    
    void HandleOKCallback() {
      on_finished_handler(this);
    }
    
    template <class... T>
    void Call(T... t) {
      callback->Call(t...);
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
    static void Queue(v8::Local<v8::Value> v8_callback, ExecuteHandler handler,
      v8::Local<v8::Object> parent = v8::Local<v8::Object>());
  };
  
  template<class T>
  Worker::OnFinishedHandler PrimitiveReturnHandler(T val) {
    return [=](Worker* worker) {
      Nan::HandleScope scope;
      v8::Local<v8::Value> argv[] = { Nan::Null(), Nan::New(val) };
      worker->Call(2, argv);
    };
  }
  template<class T, class Y>
  Worker::OnFinishedHandler InstanceReturnHandler(Y val) {
    return [=](Worker* worker) {
      Nan::HandleScope scope;
      Local<Object> childObject = T::NewInstance(val);
      Local<Value> parentObject = worker->GetFromPersistent("parent");
      if (parentObject->IsObject()) {
        childObject->Set(Nan::New("_parent").ToLocalChecked(), parentObject);
      }
      T *child = Nan::ObjectWrap::Unwrap<T>(childObject);
      NLVObjectBasePtr *childPtr = new NLVObjectBasePtr(child);
      child->SetParentReference(childPtr);
      //parent_->children_.push_back(childPtr);
      v8::Local<v8::Value> argv[] = { Nan::Null(), childObject };
      worker->Call(2, argv);
    };
  }
};
