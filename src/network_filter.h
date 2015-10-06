// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef NETWORK_FILTER_H
#define NETWORK_FILTER_H

#include "nlv_object.h"
#include "nlv_async_worker.h"
#include "worker_macros.h"

#include "hypervisor.h"

namespace NLV {

struct NetworkFilterCleanupHandler {
  static int cleanup(virNWFilterPtr handle) {
    return virNWFilterFree(handle);
  }
};

class NetworkFilter : public NLVObject<virNWFilterPtr, NetworkFilterCleanupHandler>
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virNWFilterPtr handle);

private:
  explicit NetworkFilter(virNWFilterPtr handle);
  static Nan::Persistent<Function> constructor;
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
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByName, NetworkFilter, Hypervisor, virNWFilterPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByUUID, NetworkFilter, Hypervisor, virNWFilterPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Define, NetworkFilter, Hypervisor, virNWFilterPtr);

  // WORKER METHODS
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, virNWFilterPtr, bool);
  NLV_STRING_RETURN_WORKER(GetName, virNWFilterPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetUUID, virNWFilterPtr, std::string);
  NLV_STRING_RETURN_WORKER(ToXml, virNWFilterPtr, std::string);

};

}  //namespace NLV

#endif  // NETWORK_FILTER_H
