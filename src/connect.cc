// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "connect.h"

namespace NodeLibvirt {

    void Connection::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
 
        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        //NODE_SET_PROTOTYPE_METHOD(t, "getBaselineCPU", 
        //                              Connection::GetBaselineCPU);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypCapabilities", 
        //                              Connection::GetHypervisorCapabilities);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypHostname", 
        //                              Connection::GetHypervisorHostname);
        //NODE_SET_PROTOTYPE_METHOD(t, "getRemoteLibVirtVersion", 
        //                              Connection::GetRemoteLibVirtVersion);
        //NODE_SET_PROTOTYPE_METHOD(t, "getMaxVcpus", 
        //                              Connection::GetMaxVcpus);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorType", 
        //                              Connection::GetHypervisorType);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorUri", 
        //                              Connection::GetHypervisorUri);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorVersion", 
        //                              Connection::GetHypervisorVersion);
        //NODE_SET_PROTOTYPE_METHOD(t, "isEncrypted", 
        //                              Connection::IsEncrypted);
        //NODE_SET_PROTOTYPE_METHOD(t, "isSecure", 
        //                              Connection::IsSecure);
        
        target->Set(String::NewSymbol("Connection"), t->GetFunction());
    }
    
    v8::Handle<v8::Value> Connection::New(const v8::Arguments& args) {
        v8::HandleScope scope;

        Connection *c = new Connection();
        c->Wrap(args.This());
        
        return args.This();
    }
    
    v8::Handle<v8::Value> Connection::GetHypervisorCapabilities(const v8::Arguments& args) {
        v8::HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());

        return args.This();
    }
    
} //namespace NodeLibvirt

