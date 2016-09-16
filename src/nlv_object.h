#ifndef NLV_OBJECT_H
#define NLV_OBJECT_H

#include <vector>
#include <assert.h>

#include <memory>
#include "nlv_base.h"

#include "worker.h"
using namespace node;
using namespace v8;

namespace NLV {
  NAN_INLINE unsigned int GetFlags(v8::Local<v8::Value> val) {
    if(val->IsUndefined() || val->IsFunction()) {
      return 0;
    }
    if(val->IsArray()) {
      Local<Array> flags_ = Local<Array>::Cast(val);
      unsigned int flags = 0;
      for (unsigned int i = 0; i < flags_->Length(); i++)
        flags |= flags_->Get(Nan::New<Integer>(i))->Int32Value();
      return flags;
    } else if(val->IsNumber()) {
      return val->IntegerValue();
    } else {
      Nan::ThrowTypeError("flags must be an array or a number");
      return 0;
    }
  }

  template <typename ParentClass, typename HandleType, typename CleanupHandler>
  class NLVObject : public NLVObjectBase
  {
  public:
    typedef HandleType handle_type;

    typedef typename std::remove_pointer<HandleType>::type HandleValue;

    NLVObject(HandleType handle) : handle_(handle, CleanupHandler::cleanup), parentReference_(NULL) {}
    ~NLVObject() {
      // calling virtual ClearHandle() will break if overridden by subclasses
      ClearHandle();
      if (parentReference_ != NULL) {
        parentReference_->SetInvalid();
      }
    }

    struct MethodNoReturnHandler {
      template<typename T>
      Worker::OnFinishedHandler operator()(T val) {
        return NLV::PrimitiveReturnHandler(true);
      }
    };

    template<typename T>
    struct MethodReturn {
      Worker::OnFinishedHandler operator()(T val) {
        return NLV::PrimitiveReturnHandler(static_cast<T>(val));
      }
    };
    using MethodReturnBool = MethodReturn<bool>;
    using MethodReturnInt = MethodReturn<int>;

    struct MethodReturnString {
      Worker::OnFinishedHandler operator()(const char* val) {
        std::string str = val;
        return [=](Worker* worker) {
          Nan::HandleScope scope;
          v8::Local<v8::Value> argv[] = { Nan::Null(), Nan::New(str).ToLocalChecked() };
          worker->Call(2, argv);
        };
      }
    };
    
    static inline bool virHasFailed(int val) {
      return val < 0;
    }
    
    static inline bool virHasFailed(const void* ptr) {
      return ptr == nullptr;
    }

    template<typename ReturnHandler = MethodNoReturnHandler, typename Z, typename... Args>
    static void RunMethod(const Nan::FunctionCallbackInfo<v8::Value>& info, Z virFunction, Args... args) {
      Nan::HandleScope scope;
      virDomainPtr handle = ParentClass::Unwrap(info.This())->virHandle();
      auto virCaller = std::bind(virFunction, handle, args...);
      NLV::Worker::RunAsync(info, [=] (NLV::Worker::SetOnFinishedHandler onFinished) {
        auto retVal = virCaller();
        if (virHasFailed(retVal)) {
          return virSaveLastError();
        }
        ReturnHandler returnHandler;
        return onFinished(returnHandler(retVal));
      });
    }

    static v8::Local<v8::Object> NewInstance(handle_type handle) {
      Nan::EscapableHandleScope scope;
      Local<Function> ctor = Nan::New<Function>(ParentClass::constructor);
      Local<Object> object = Nan::NewInstance(ctor).ToLocalChecked();
      ParentClass *class_instance = new ParentClass(handle);
      class_instance->Wrap(object);
      return scope.Escape(object);
    }

    static bool IsInstanceOf(v8::Local<v8::Object> val) {
      Nan::HandleScope scope;
      return Nan::New(ParentClass::constructor_template)->HasInstance(val);
    }

    const HandleType virHandle() const {
      return handle_.get();
     }


    NAN_INLINE static ParentClass* Unwrap(v8::Local<v8::Object> val) {
      if(!ParentClass::IsInstanceOf(val)) {
        char error[128];
        snprintf(error, sizeof(error), "Expecting object to be %s", ParentClass::ClassName());
        Nan::ThrowTypeError(error);
        return nullptr;
      }

      return ObjectWrap::Unwrap<ParentClass>(val);
    }

    NAN_INLINE static ParentClass* Unwrap(v8::Local<v8::Value> val) {
      if(!val->IsObject()) {
        char error[128];
        snprintf(error, sizeof(error), "Cannot unwrap handle from non-object, expecting %s", ParentClass::ClassName());
        Nan::ThrowTypeError(error);
        return nullptr;
      }

      return Unwrap(val->ToObject());
    }

    NAN_INLINE static HandleType UnwrapHandle(v8::Local<v8::Value> val) {
      return Unwrap(val)->virHandle();
    }

    template<class T>
    NAN_INLINE static HandleType UnwrapHandle(v8::Local<v8::Object> val) {
      return Unwrap(val)->virHandle();
    }

    virtual void ClearHandle() {
      handle_.reset();
    }

    virtual void ClearChildren() {
      std::vector<NLVObjectBasePtr*>::const_iterator it;
      for (it = children_.begin(); it != children_.end(); ++it) {
        NLVObjectBasePtr *ptr = *it;
        if (ptr->IsValid()) {
          NLVObjectBase *obj = ptr->GetPointer();
          obj->ClearChildren();
          obj->ClearHandle();
          obj->SetParentReference(NULL);
          delete ptr;
        }
      }

      children_.clear();
    }

    virtual void SetParentReference(NLVObjectBasePtr *parentReference) {
      parentReference_ = parentReference;
    }

  protected:
    std::unique_ptr<HandleValue, decltype(&CleanupHandler::cleanup)> handle_;
    NLVObjectBasePtr* parentReference_;

  };

  NAN_INLINE void AsyncQueueWorker(Nan::AsyncWorker *worker, Local<Object> parent = Local<Object>())
  {
    if (!parent->IsUndefined() && !parent->IsNull())
      worker->SaveToPersistent("parent", parent);
    Nan::AsyncQueueWorker(worker);
  }
};

#endif  // NLV_OBJECT_H
