#ifndef NLV_OBJECT_H
#define NLV_OBJECT_H

#include <vector>
#include <assert.h>

#include <memory>
#include "nlv_base.h"

#include "worker.h"

#include "utility.hpp"

using namespace node;
using namespace v8;

namespace NLV {
  typedef std::basic_string<unsigned char> ustring;

  NAN_INLINE std::string GetString(v8::Local<v8::Value> val) {
    if(!val->IsString()) {
      Nan::ThrowTypeError("argument should be a string");
    }
    return std::string(*Nan::Utf8String(val->ToString()));
  }

  NAN_INLINE ustring GetUString(v8::Local<v8::Value> val) {
    if(!val->IsString()) {
      Nan::ThrowTypeError("argument should be a string");
    }
    auto valString = val->ToString();
    return ustring(reinterpret_cast<const unsigned char*>(*Nan::Utf8String(valString)));
  }

  NAN_INLINE std::shared_ptr<std::string> MakeOutString(size_t size) {
    return std::make_shared<std::string>(size + 1, '\0');
  }

  template<typename T, typename ...Args>
  NAN_INLINE std::shared_ptr<T> MakeOut(Args&&... args) {
    return std::make_shared<T>(args...);
  }

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

  template<typename T, typename Y = T>
  inline Y&& WrapArg(T&& val) {
    return val;
  }

  struct Unwrapper {
    template<typename T>
    inline T operator()(T arg) {
      return arg;
    }

    inline const char* operator()(const std::string& arg) {
      return arg.c_str();
    }

    inline const unsigned char* operator()(const ustring& arg) {
      return arg.c_str();
    }

    inline char* operator()(std::shared_ptr<std::string>& str) {
      return (char*)str->c_str();
    }

    template<typename T>
    inline T* operator()(std::shared_ptr<T>& val) {
      return val.get();
    }
  };

  struct ReturnUnwrapper {
    template<typename T>
    inline T&& operator()(T&& arg) {
      return arg;
    }

    inline char* operator()(std::shared_ptr<std::string>& str) {
      return (char*)str->c_str();
    }

    template<typename T>
    inline T operator()(std::shared_ptr<T>& val) {
      return *val;
    }
  };

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

      Worker::OnFinishedHandler operator()(const std::shared_ptr<std::string> &ptr) {
        return NLV::PrimitiveReturnHandler(static_cast<T>(*ptr));
      }

      Worker::OnFinishedHandler operator()(const std::shared_ptr<int> &ptr) {
        return NLV::PrimitiveReturnHandler(static_cast<T>(*ptr));
      }
    };
    using MethodReturnString = MethodReturn<const char*>;

    template<typename T>
    struct MethodReturnInstance {
      using handle_type = typename T::handle_type;
      Worker::OnFinishedHandler operator()(handle_type handle) {
        return NLV::InstanceReturnHandler<T>(handle);
      }
    };

    static inline bool virHasFailed(int val) {
      return val == -1;
    }

    static inline bool virHasFailed(const void* ptr) {
      return ptr == nullptr;
    }

    template<typename ReturnHandler = MethodNoReturnHandler, int ReturnIndex = 0, typename Z,
    typename... Args>
    static void RunMethod(const Nan::FunctionCallbackInfo<v8::Value>& info, Z virFunction, Args&&... args) {
      Nan::HandleScope scope;
      auto handle = ParentClass::Unwrap(info.This())->virHandle();

      /*
        we're saving all arguments, wrapping them in safely copyable types, like:
        const char* -> std::string

        different wrapping methods are defined by specialization/overloading of WrapArg
        - by default WrapArg is just passthrough
      */
      auto tuple = std::make_tuple(handle, WrapArg(args)...);
      NLV::Worker::RunAsync(info, [=] (NLV::Worker::SetOnFinishedHandler onFinished) {
        // here we're running in non-v8 thread and the tuple has been copied by value
        // now we need to call virFunction with all the arguments
        // arguments are gonna be unwrapped using Unwrapper class with different
        // specialization/overloads, for example:
        // std::string -> const char*
        // std::shared_ptr<std::string> -> char*   <- used for returning by pointer
        auto retVal = utility::apply<Unwrapper>(virFunction, tuple);
        if (virHasFailed(retVal)) {
          return virSaveLastError();
        }
        ReturnHandler returnHandler;

        // this tuple contains:
        // 0 - virFunction return value
        // 1 - handle
        // 2 - first arugment after handle
        // by default index 0 will be passed to javascript
        // but by passing ReturnIndex > 1 we can get values returned by pointer
        auto retTuple = std::tuple_cat(std::make_tuple(retVal), tuple);

        return onFinished(returnHandler(ReturnUnwrapper()(std::get<ReturnIndex>(retTuple))));
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
