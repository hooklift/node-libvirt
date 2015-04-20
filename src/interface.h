// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_INTERFACE_H_
#define SRC_INTERFACE_H_

#include "node_libvirt.h"

#include "worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class Interface : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(const LibVirtHandle &handle);
  virtual ~Interface();

  virInterfacePtr GetInterface() const;

private:
  static Persistent<Function> constructor;
  virInterfacePtr handle_;

  friend class Hypervisor;

private:
  explicit Interface(virInterfacePtr handle) : handle_(handle) {}

  // HYPERVISOR METHODS
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByMacAddress);
  static NAN_METHOD(Define);

  // METHODS
  static NAN_METHOD(Start);
  static NAN_METHOD(Stop);
  static NAN_METHOD(Undefine);
  static NAN_METHOD(IsActive);
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetMacAddress);
  static NAN_METHOD(ToXml);

private:
  // HYPERVISOR METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(Interface, LookupByName);
  NLV_LOOKUP_BY_VALUE_WORKER(Interface, LookupByMacAddress);
  NLV_LOOKUP_BY_VALUE_WORKER(Interface, Define);

  // WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Start, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Stop, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsActive, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetName, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetMacAddress, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, std::string);


};

}  // namespace NodeLibvirt

#endif  // SRC_INTERFACE_H_
