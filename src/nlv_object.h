#ifndef NLV_OBJECT_H
#define NLV_OBJECT_H

#include <vector>
#include <assert.h>

#include <nan.h>
using namespace node;
using namespace v8;

class NLVObjectBase : public node::ObjectWrap
{
public:
  virtual void ClearHandle() = 0;
  virtual void ClearChildren() = 0;
};

template <typename HandleType, typename CleanupHandler>
class NLVObject : public NLVObjectBase
{
public:
  NLVObject(HandleType handle) : handle_(handle) {}
  ~NLVObject() {
    ClearChildren();
    ClearHandle();
  }

  virtual void ClearHandle() {
    if (handle_ != NULL) {
      int result = CleanupHandler::cleanup(handle_);
      assert(result == 0);
      handle_ = NULL;
    }
  }

  virtual void ClearChildren() {
    std::vector<NLVObjectBase*>::const_iterator it;
    for (it = children_.begin(); it != children_.end(); ++it) {
      (*it)->ClearChildren();
      (*it)->ClearHandle();
    }
  }

  std::vector<NLVObjectBase*> children_;

protected:
  HandleType handle_;

};

#endif  // NLV_OBJECT_H
