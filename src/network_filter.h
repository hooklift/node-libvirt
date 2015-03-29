// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NETWORK_FILTER_H_
#define SRC_NETWORK_FILTER_H_

#include <nan.h>

namespace NodeLibvirt {

class NetworkFilter : public ObjectWrap
{
public:
    static void Initialize();

private:
    static Persistent<FunctionTemplate> constructor_template;
    virNWFilterPtr filter_;

protected:
    static NAN_METHOD(LookupByName);
    static NAN_METHOD(LookupByUUID);
    static NAN_METHOD(Define);
    static NAN_METHOD(Undefine);
    static NAN_METHOD(GetName);
    static NAN_METHOD(GetUUID);
    static NAN_METHOD(ToXml);

};

}  //namespace NodeLibvirt

#endif  // SRC_NETWORK_FILTER_H_

