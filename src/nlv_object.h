#ifndef NLV_OBJECT_H
#define NLV_OBJECT_H

#include <vector>
#include <assert.h>

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

class NLVObjectBase : public Nan::ObjectWrap
{
public:
  virtual void ClearHandle() = 0;
  virtual void ClearChildren() = 0;
  virtual void SetParentReference(NLVObjectBasePtr *parentReference) = 0;
};

template <typename HandleType, typename CleanupHandler>
class NLVObject : public NLVObjectBase
{
public:
  NLVObject(HandleType handle) : handle_(handle), parentReference_(NULL) {}
  ~NLVObject() {
    // calling virtual ClearHandle() will break if overridden by subclasses
    ClearHandle();
    if (parentReference_ != NULL) {
      parentReference_->SetInvalid();
    }
  }

  virtual void ClearHandle() {
    if (handle_ != NULL) {
      int result = CleanupHandler::cleanup(handle_);
      assert(result == 0);
      handle_ = NULL;
    }
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

  std::vector<NLVObjectBasePtr*> children_;

protected:
  HandleType handle_;
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
