// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NETWORK_FILTER_H_
#define SRC_NETWORK_FILTER_H_

#include <nan.h>

#include "nlv_async_worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class NetworkFilter : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virNWFilterPtr handle);
  virtual ~NetworkFilter();

private:
  explicit NetworkFilter(virNWFilterPtr handle) : handle_(handle) {}
  static Persistent<Function> constructor;
  virNWFilterPtr handle_;

  friend class Hypervisor;

protected:
  // HYPERVISOR METHODS
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByUUID);
  static NAN_METHOD(Define);

  // METHODS
  static NAN_METHOD(Undefine);
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetUUID);
  static NAN_METHOD(ToXml);

private:
  // HYPERVISOR WORKER METHODS
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByName, NetworkFilter, virConnectPtr, virNWFilterPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByUUID, NetworkFilter, virConnectPtr, virNWFilterPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Define, NetworkFilter, virConnectPtr, virNWFilterPtr);

  // WORKER METHODS
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, virNWFilterPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetName, virNWFilterPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetUUID, virNWFilterPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, virNWFilterPtr, std::string);

};

}  //namespace NodeLibvirt

#endif  // SRC_NETWORK_FILTER_H_
