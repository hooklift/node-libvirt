// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NETWORK_FILTER_H_
#define SRC_NETWORK_FILTER_H_

#include <nan.h>

#include "worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class NetworkFilter : public ObjectWrap
{
public:
  static void Initialize();
  static Local<Object> NewInstance(const LibVirtHandle &handle);
  virtual ~NetworkFilter();

private:
  explicit NetworkFilter(virNWFilterPtr handle) : handle_(handle) {}
  static Persistent<FunctionTemplate> constructor_template;
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
  NLV_LOOKUP_BY_VALUE_WORKER(NetworkFilter, LookupByName);
  NLV_LOOKUP_BY_VALUE_WORKER(NetworkFilter, LookupByUUID);
  NLV_LOOKUP_BY_VALUE_WORKER(NetworkFilter, Define);

  // WORKER METHODS
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetName, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetUUID, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, std::string);

};

}  //namespace NodeLibvirt

#endif  // SRC_NETWORK_FILTER_H_

