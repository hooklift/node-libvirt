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

  void ClearHandle() {
    if (handle_ != NULL) {
      int result = CleanupHandler::cleanup(handle_);
      assert(result == 0);
      handle_ = NULL;
    }
  }

  void ClearChildren() {
    std::vector<NLVObjectBase*>::const_iterator it;
    for (it = children_.cbegin(); it != children_.cend(); ++it) {
      (*it)->ClearHandle();
    }
  }

protected:
  std::vector<NLVObjectBase*> children_;
  HandleType handle_;

};

#endif  // NLV_OBJECT_H
