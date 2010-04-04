// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdio.h>
#include <stdlib.h>
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
        NODE_SET_PROTOTYPE_METHOD(t, "getHypCapabilities", 
                                      Connection::GetHypervisorCapabilities);
        NODE_SET_PROTOTYPE_METHOD(t, "getHypHostname", 
                                      Connection::GetHypervisorHostname);
        NODE_SET_PROTOTYPE_METHOD(t, "getRemoteLibVirtVersion", 
                                      Connection::GetRemoteLibVirtVersion);
        NODE_SET_PROTOTYPE_METHOD(t, "getMaxVcpus", 
                                      Connection::GetMaxVcpus);
        NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorType", 
                                      Connection::GetHypervisorType);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorUri", 
        //                              Connection::GetHypervisorUri);
        //NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorVersion", 
        //                              Connection::GetHypervisorVersion);
        //NODE_SET_PROTOTYPE_METHOD(t, "isEncrypted", 
        //                              Connection::IsEncrypted);
        //NODE_SET_PROTOTYPE_METHOD(t, "isSecure", 
        //                              Connection::IsSecure);
        NODE_SET_PROTOTYPE_METHOD(t, "close", 
                                      Connection::Close);
        
        target->Set(String::NewSymbol("Connection"), t->GetFunction());
    }
    
    Connection::Connection(const v8::Local<v8::String>& uriStr, bool readOnly) : EventEmitter(){
        v8::String::Utf8Value uriUtf8(uriStr);
        const char *uri = ToCString(uriUtf8);
        //TODO auth support
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
    
    v8::Handle<v8::Value> Connection::get_hypervisor_capabilities() {
        char *cap = virConnectGetCapabilities(conn);
        
        if(cap == NULL) {
            LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor capabilities");
        }
        
        v8::Local<v8::String> capabilities = v8::String::New((const char*)cap);
        delete cap;
        
        return capabilities;
    }
    
    v8::Handle<v8::Value> Connection::GetHypervisorHostname(const v8::Arguments& args) {
        v8::HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        
        return connection->get_hypervisor_hostname();
    }
    
    v8::Handle<v8::Value> Connection::get_hypervisor_hostname() {
        char *hn = virConnectGetHostname(conn);
        
        if(hn == NULL) {
            LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor hostname");
        }
        
        v8::Local<v8::String> hostname = v8::String::New((const char*)hn);
        delete hn;
        
        return hostname;
    }
    
    v8::Handle<v8::Value> Connection::Close(const v8::Arguments& args) {
        v8::HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->close();
    }
    
    v8::Handle<Value> Connection::close() {
        int isClosed = -1;
        if(conn != NULL) {
            isClosed = virConnectClose(conn);

            if(isClosed == -1) {
                return False();
            }
        }
        conn = NULL;
        return True();
    }
    
    v8::Handle<v8::Value> Connection::GetRemoteLibVirtVersion(const v8::Arguments& args) {
        v8::HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->get_remote_libvirt_version();
    }
    
    v8::Handle<v8::Value> Connection::get_remote_libvirt_version() {
        unsigned long *libVer;
        
        libVer = new unsigned long;
        
        int ret = virConnectGetLibVersion(conn, libVer);
        
        if(ret == -1) {
            LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive remote libvirt version");
        }
        
        v8::Local<v8::Number> version = v8::Number::New((double)*libVer);
        delete libVer;
        
        return version;
    }
    
    
    v8::Handle<v8::Value> Connection::GetHypervisorType(const v8::Arguments& args) {
        v8::HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->get_hypervisor_type();
    }
    
    v8::Handle<v8::Value> Connection::get_hypervisor_type() {
        const char *t = virConnectGetType(conn);
        
        if(t == NULL) {
            LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor type");
        }
        
        v8::Local<v8::String> type = v8::String::New(t);
        
        return type;
    }
    
     v8::Handle<v8::Value> Connection::GetMaxVcpus(const v8::Arguments& args) {
        v8::HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->get_max_vcpus();
    }
    
    v8::Handle<v8::Value> Connection::get_max_vcpus() {
        const char *type = virConnectGetType(conn);
        
        if(type == NULL) {
            LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor type");
        }
        
        int m = virConnectGetMaxVcpus(conn, type);
        
        if(m == -1) {
            LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive maximum number of CPUs supported");
        }
        
        v8::Local<v8::Number> max = v8::Number::New(m);
        
        return max;
    }
    
} //namespace NodeLibvirt

