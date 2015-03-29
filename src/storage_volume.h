// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_STORAGE_VOLUME_H_
#define SRC_STORAGE_VOLUME_H_

#include <nan.h>

namespace NodeLibvirt {

class StorageVolume : public ObjectWrap
{
public:
  static void Initialize();

private:
  static Persistent<FunctionTemplate> constructor_template;
  virStorageVolPtr volume_;

  friend class StoragePool;

protected:
  static NAN_METHOD(Create);
  static NAN_METHOD(GetInfo);
  static NAN_METHOD(GetKey);
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetPath);
  static NAN_METHOD(ToXml);
  static NAN_METHOD(LookupByKey);
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByPath);
  static NAN_METHOD(Wipe);
  static NAN_METHOD(Delete);
  static NAN_METHOD(Clone);

};

}  //namespace NodeLibvirt

#endif  // SRC_STORAGE_VOLUME_H

