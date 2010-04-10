// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connect.h"

using namespace v8;

namespace NodeLibvirt {

    //Persistent<FunctionTemplate> Connection::constructor_template;

    void Connection::Initialize(Handle<Object> target) {
        HandleScope scope;
        
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
 
        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        NODE_SET_PROTOTYPE_METHOD(t, "getBaselineCPU", 
                                      Connection::GetBaselineCPU);
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
        NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorUri", 
                                      Connection::GetHypervisorUri);
        NODE_SET_PROTOTYPE_METHOD(t, "getHypervisorVersion", 
                                      Connection::GetHypervisorVersion);
        NODE_SET_PROTOTYPE_METHOD(t, "isEncrypted", 
                                      Connection::IsEncrypted);
        NODE_SET_PROTOTYPE_METHOD(t, "isSecure", 
                                      Connection::IsSecure);
        NODE_SET_PROTOTYPE_METHOD(t, "close", 
                                      Connection::Close);
        
        target->Set(String::NewSymbol("Connection"), t->GetFunction());
    }
    
    Connection::Connection(const Local<String>& uriStr, bool readOnly) : EventEmitter(){
        HandleScope scope;
        String::Utf8Value uriUtf8(uriStr);
        const char *uri = ToCString(uriUtf8);
        //TODO auth support
        if(readOnly) {
            conn = virConnectOpenReadOnly(uri);
        } else {
            conn = virConnectOpen(uri);
        }
        
        if(conn == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
        }
    }
      
    Connection::~Connection(){
        assert(conn == NULL);
    }
    
    Handle<Value> Connection::New(const Arguments& args) {
        HandleScope scope;
        
        if(args.Length() == 0 ) {
            return ThrowException(Exception::TypeError(
            String::New("You need specify at least a Hypervisor URI")));    
        }
        
        if(!args[0]->IsString()) { 
            return ThrowException(Exception::TypeError(
            String::New("First argument must be a string")));
        }
        
        if(args.Length() == 2 && !args[1]->IsBoolean()) {
            return ThrowException(Exception::TypeError(
            String::New("Second argument must be a boolean")));
        }
        
        Connection *c = new Connection(args[0]->ToString(), args[1]->IsTrue());
        c->Wrap(args.This());
        
        return args.This();
    }
    
    Handle<Value> Connection::GetHypervisorCapabilities(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());

        return connection->get_hypervisor_capabilities(); 
    }
    
    Handle<Value> Connection::get_hypervisor_capabilities() {
        char *cap = virConnectGetCapabilities(conn);
        
        if(cap == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor capabilities"); */
        }
        
        Local<String> capabilities = String::New((const char*)cap);
        delete cap;
        
        return capabilities;
    }
    
    Handle<Value> Connection::GetHypervisorHostname(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        
        return connection->get_hypervisor_hostname();
    }
    
    Handle<Value> Connection::get_hypervisor_hostname() {
        char *hn = virConnectGetHostname(conn);
        
        if(hn == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor hostname");*/
        }
        
        Local<String> hostname = String::New((const char*)hn);
        delete hn;
        
        return hostname;
    }
    
    Handle<Value> Connection::Close(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->close();
    }
    
    Handle<Value> Connection::close() {
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
    
    Handle<Value> Connection::GetRemoteLibVirtVersion(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->get_remote_libvirt_version();
    }
    
    Handle<Value> Connection::get_remote_libvirt_version() {
        unsigned long *libVer;
        
        libVer = new unsigned long;
        
        int ret = virConnectGetLibVersion(conn, libVer);
        
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive remote libvirt version");*/
        }
        
        Local<Number> version = Number::New((double)*libVer);
        delete libVer;
        
        return version;
    }
    
    
    Handle<Value> Connection::GetHypervisorType(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->get_hypervisor_type();
    }
    
    Handle<Value> Connection::get_hypervisor_type() {
        const char *t = virConnectGetType(conn);
        
        if(t == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor type");*/
        }
        
        Local<String> type = String::New(t);
        
        return type;
    }
    
    Handle<Value> Connection::GetMaxVcpus(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->get_max_vcpus();
    }
    
    Handle<Value> Connection::get_max_vcpus() {
        const char *type = virConnectGetType(conn);
        
        if(type == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor type");*/
        }
        
        int m = virConnectGetMaxVcpus(conn, type);
        
        if(m == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive maximum number of CPUs supported");*/
        }
        
        Local<Number> max = Number::New(m);
        
        return max;
    }
    
    Handle<Value> Connection::GetHypervisorUri(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->get_hypervisor_uri();
    }
    
    Handle<Value> Connection::get_hypervisor_uri() {
        char *u = virConnectGetURI(conn);
        
        if(u == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive hypervisor connection URI");*/
        }
         
        Local<String> uri = String::New(u);
        delete u;
        
        return uri;
    }
    
    Handle<Value> Connection::GetHypervisorVersion(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->get_hypervisor_version();
    }
    
    Handle<Value> Connection::get_hypervisor_version() {
        unsigned long *hvVer;
        
        hvVer = new unsigned long;
        
        int ret = virConnectGetVersion(conn, hvVer);
        
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to retrive Hypervisor version");*/
        }
        
        if(ret == 0 && *hvVer == 0) {
            delete hvVer;
            return Null(); 
            /*LIBVIRT_THROW_EXCEPTION(
                "Hypervisor lack of capacities to retrive its version");*/
        }
        
        Local<Number> version = Number::New((double)*hvVer);
        delete hvVer;

        return version;
    }
    
    Handle<Value> Connection::IsEncrypted(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->is_encrypted();
    }
    
    Handle<Value> Connection::is_encrypted() {
        int ret = virConnectIsEncrypted(conn);
        
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to determinate if Hypervisor connection is encrypted");*/
        }
        
        if(ret == 1) {
            return True();
        }

       return False();
    }
    
    Handle<Value> Connection::IsSecure(const Arguments& args) {
        HandleScope scope;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        return connection->is_secure();
    }
    
    Handle<Value> Connection::is_secure() {
        int ret = virConnectIsSecure(conn);
        
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            /*LIBVIRT_THROW_EXCEPTION(
            "There was an error while attempting to determinate if Hypervisor connection is secure");*/
        }
        
        if(ret == 1) {
            return True();
        }

       return False();
    }
    
     Handle<Value> Connection::GetBaselineCPU(const Arguments& args) {
        HandleScope scope;
        
        const char **xmlCPUs = NULL;
        unsigned int ncpus = 0;
        unsigned int flags = 0;
        
        Connection *connection = ObjectWrap::Unwrap<Connection>(args.This());
        
        if(args.Length() == 0 || !args[0]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You need to specify an Array with two xml's to compute the most feature-rich CPU")));    
        }
        
        Local<Array> xmls = Local<Array>::Cast(args[0]);
        ncpus = xmls->Length();
        char **xmls1 = new char*[ncpus + 1];
        xmls1[ncpus] = NULL;
        for (int i = 0; i < ncpus; i++) {
            String::Utf8Value cpu(xmls->Get(Integer::New(i))->ToString());
            xmls1[i] = strdup(*cpu);
        }
        
        return connection->get_baseline_cpu(xmls1, ncpus, flags); 
    }
    
    Handle<Value> Connection::get_baseline_cpu( char **xmlCPUs, 
                                                unsigned int ncpus, 
                                                unsigned int flags) {  
        const char *x = virConnectBaselineCPU(conn, (const char**)xmlCPUs, ncpus, flags);
        
        if(x == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
/*            LIBVIRT_THROW_EXCEPTION(
                "There was an error while attempting to compute the most feature-rich CPU, which is compatible with all given host CPUs hypervisor capabilities"); */
        }
        
        for (int i = 0; i < ncpus; i++) {
            free(xmlCPUs[i]);
        }
         
        delete [] xmlCPUs;
        
        Local<String> xml = String::New(x);
        
        return xml;
    }
    
} //namespace NodeLibvirt
