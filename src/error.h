// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef ERROR_H
#define ERROR_H

#include "node_libvirt.h"

namespace NLV {

class Error : public Nan::ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Value> New(virErrorPtr error, std::string context);

private:
  explicit Error(virErrorPtr error, std::string context);
  ~Error();
  static Nan::Persistent<Function> constructor;

  static NAN_GETTER(Getter);

  virErrorPtr error_;
  std::string context_;
};

} //namespace NLV

#define _LIBVIRT_STRINGIZE_DETAIL(x) #x
#define _LIBVIRT_STRINGIZE(x) _LIBVIRT_STRINGIZE_DETAIL(x)
#define ThrowLastVirError() \
  Nan::ThrowError(Error::New(virSaveLastError(), __FILE__ ":" _LIBVIRT_STRINGIZE(__LINE__)))
#define SET_ERROR_WITH_CONTEXT(error) \
  SetVirError(error, __FILE__ ":" _LIBVIRT_STRINGIZE(__LINE__))

#endif // ERROR_H
