// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "connect.h"

namespace NodeLibvirt {

    void Connection::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
 
        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        //NODE_SET_PROTOTYPE_METHOD(t, "getBaselineCPU", 
        //                              GetBaselineCPU);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypCapabilities", 
        //                              GetHypCapabilities);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypHostname", 
        //                              GetHypHostname);
        //NODE_SET_PROTOTYPE_METHOD(t, "getRemoteLibVirtVersion", 
        //                              GetRemoteLibVirtVersion);
        //NODE_SET_PROTOTYPE_METHOD(t, "getMaxVcpus", 
        //                              GetMaxVcpus);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorType", 
        //                              GetHypervisorType);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorUri", 
        //                              GetHypervisorUri);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorVersion", 
        //                              GetHypervisorVersion);
        //NODE_SET_PROTOTYPE_METHOD(t, "isEncrypted", 
        //                              IsEncrypted);
        //NODE_SET_PROTOTYPE_METHOD(t, "isSecure", 
        //                              IsSecure);
        
        target->Set(String::NewSymbol("Connection"), t->GetFunction());
    }
    
    v8::Handle<v8::Value> Connection::New(const v8::Arguments& args) {
        v8::HandleScope scope;

        Connection *c = new Connection();
        c->Wrap(args.This());
        
        return args.This();
    }    
    
} //namespace NodeLibvirt

