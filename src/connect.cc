// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "connect.h"

namespace NodeLibvirt {

    //v8::Persistent<v8::FunctionTemplate> Connection::constructor_template;

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
    
    Connection::Connection(const v8::Local<v8::String>& uriStr, bool readOnly) : EventEmitter(){
        v8::String::Utf8Value uriUtf8(uriStr);
        const char *uri = ToCString(uriUtf8);
        
        if(readOnly) {
            conn = virConnectOpenReadOnly(uri);
        } else {
            conn = virConnectOpen(uri);
        }
    }
 
    Connection::~Connection(){
        assert(conn == NULL);
    }
    
    v8::Handle<v8::Value> Connection::New(const v8::Arguments& args) {
        v8::HandleScope scope;
        
        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("First argument must be a string")));
        }
        
        Connection *c = new Connection(args[0]->ToString(), args[1]->IsTrue());
        c->Wrap(args.This());
        
        return args.This();
    }
    
    v8::Handle<v8::Value> Connection::GetHypervisorCapabilities(const v8::Arguments& args) {
        v8::HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());

        return connection->get_hypervisor_capabilities(); 
    }
    
    v8::Handle<v8::String> Connection::get_hypervisor_capabilities() {
        char *cap = virConnectGetCapabilities(conn);
        
        if(cap == NULL) {
            LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor capabilities");
        }
        
        return v8::String::New((const char*)cap);
    }
    
} //namespace NodeLibvirt

