// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_STORAGE_POOL_H_
#define SRC_STORAGE_POOL_H_

#include "node_libvirt.h"

namespace NodeLibvirt {

class StoragePool : public ObjectWrap
{
public:
  static void Initialize();

  virStoragePoolPtr Pool() const;

private:
  static Persistent<FunctionTemplate> constructor_template;
  virStoragePoolPtr pool_;

  friend class StorageVolume;
  friend class Hypervisor;

private:
  static NAN_METHOD(Build);
  static NAN_METHOD(Create);
  static NAN_METHOD(Define);
  static NAN_METHOD(Undefine);
  static NAN_METHOD(Start);
  static NAN_METHOD(Stop);
  static NAN_METHOD(Erase);
  static NAN_METHOD(GetAutostart);
  static NAN_METHOD(SetAutostart);
  static NAN_METHOD(GetInfo);
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetUUID);
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByUUID);
  static NAN_METHOD(LookupByVolume);
  static NAN_METHOD(ToXml);
  static NAN_METHOD(IsActive);
  static NAN_METHOD(IsPersistent);
  static NAN_METHOD(GetVolumes);
  static NAN_METHOD(Refresh);

};

}  //namespace NodeLibvirt

#endif  // SRC_STORAGE_POOL_H

