#ifndef NLV_OBJECT_H
#define NLV_OBJECT_H

#include <vector>
#include <assert.h>

#include <memory>
#include <nan.h>
#include <memory>
#include <type_traits>
#include <forward_list>

using namespace node;
using namespace v8;

class NLVObjectBase;

#define NLV_STRINGIFY0(v) #v
#define NLV_STRINGIFY(v) NLV_STRINGIFY0(v)

// macro could be removed but alternate solution seems overly complex
// https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
#define NLV_OBJECT_STATIC_HELPERS(class_name) \
  static const char* ClassName() { \
    return NLV_STRINGIFY(class_name); \
  } \
  friend class NLVObject;


class NLVObjectBase : public Nan::ObjectWrap, public std::enable_shared_from_this<NLVObjectBase>
{
public:
  void AddToParent(NLVObjectBase* parent) {
    parent->PushChild(shared_from_this());
  }
  
  void ClearChildren() {
    for (auto& ptr: children_) {
      if (auto object = ptr.lock()) {
        object->ClearHandle();
        object->ClearChildren();
      }
    }
    children_.clear();
  }

  virtual void ClearHandle() = 0;

  void PushChild(const std::shared_ptr<NLVObjectBase>& child) {
    children_.emplace_front(child);
  }
  std::forward_list<std::weak_ptr<NLVObjectBase>> children_;
};

template <typename ParentClass, typename HandleType, typename CleanupHandler>
class NLVObject : public NLVObjectBase
{
  std::shared_ptr<NLVObjectBase> selfPtr;
public:
  typedef HandleType handle_type;
  typedef typename std::remove_pointer<HandleType>::type HandleValue;
    
  NLVObject(HandleType handle) : handle_(handle, CleanupHandler::cleanup) {
  }
  
  ~NLVObject() {
  }
  
  void RegisterSelf() {
    selfPtr = shared_from_this();
  }

  static v8::Local<v8::Object> NewInstance(handle_type handle) {
    Nan::EscapableHandleScope scope;
    Local<Function> ctor = Nan::New<Function>(ParentClass::constructor);
    Local<Object> object = Nan::NewInstance(ctor).ToLocalChecked();
    auto shared = std::shared_ptr<ParentClass>(new ParentClass(handle), [=](ParentClass*) {
      // here we can now if GC has destroyed our object
    });
    ParentClass *class_instance = shared.get();
    class_instance->RegisterSelf();
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

  void ClearHandle() {
    handle_.reset();
  }

protected:
  std::unique_ptr<HandleValue, decltype(&CleanupHandler::cleanup)> handle_;
};

namespace NLV {

NAN_INLINE void AsyncQueueWorker(Nan::AsyncWorker *worker, Local<Object> parent = Local<Object>())
{
  if (!parent->IsUndefined() && !parent->IsNull())
    worker->SaveToPersistent("parent", parent);
  Nan::AsyncQueueWorker(worker);
}

NAN_INLINE unsigned int GetFlags(v8::Local<v8::Value> val) {
  if(val->IsUndefined() || val->IsFunction()) {
    return 0;
  }
  if(!val->IsArray()) {
    Nan::ThrowTypeError("flags must be an array");
    return 0;
  }

  Local<Array> flags_ = Local<Array>::Cast(val);
  unsigned int flags = 0;
  for (unsigned int i = 0; i < flags_->Length(); i++)
    flags |= flags_->Get(Nan::New<Integer>(i))->Int32Value();
  return flags;
}

};

#endif  // NLV_OBJECT_H
