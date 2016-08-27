#ifndef NLV_OBJECT_H
#define NLV_OBJECT_H

#include <vector>
#include <assert.h>

#include <memory>
#include <nan.h>
using namespace node;
using namespace v8;

class NLVObjectBase;

// hold a reference to a "child" object in a way that can be safely invalidated
// if the child is destroyed by the GC before the parent.
class NLVObjectBasePtr
{
public:
  NLVObjectBasePtr(NLVObjectBase *ref) : ref_(ref), valid_(true) {}
  bool IsValid() const { return valid_; }
  NLVObjectBase* GetPointer() const {
    if (!valid_) {
      //Nan::ThrowReferenceError("attempt to access invalid NLVObjectBase pointer");
      return NULL;
    }

    return ref_;
  }

  void SetInvalid() {
    ref_ = NULL;
    valid_ = false;
  }

protected:
  NLVObjectBase *ref_;
  bool valid_;
};

#define NLV_STRINGIFY0(v) #v
#define NLV_STRINGIFY(v) NLV_STRINGIFY0(v)

// macro could be removed but alternate solution seems overly complex
// https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
#define NLV_OBJECT_STATIC_HELPERS(class_name) \
  static const char* ClassName() { \
    return NLV_STRINGIFY(class_name); \
  } \
  friend class NLVObject;


class NLVObjectBase : public Nan::ObjectWrap
{
public:
  virtual void ClearHandle() = 0;
  virtual void ClearChildren() = 0;
  virtual void SetParentReference(NLVObjectBasePtr *parentReference) = 0;

  std::vector<NLVObjectBasePtr*> children_;
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
