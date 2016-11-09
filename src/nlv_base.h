#ifndef NLV_BASE_H
#define NLV_BASE_H

#include <nan.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#include "error.h"

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

/**
 * Base class for all workers in node-libvirt
 */
class NLVAsyncWorkerBase : public Nan::AsyncWorker
{
public:
  explicit NLVAsyncWorkerBase(Nan::Callback *callback);
  ~NLVAsyncWorkerBase();

  virErrorPtr VirError() const;
  void SetVirError(virErrorPtr error, const char *context);

  virtual void WorkComplete();

protected:
  virtual void HandleErrorCallback();

private:
  virErrorPtr virError_;
  std::string virErrorContext_;

};

#endif // NLV_BASE_H
