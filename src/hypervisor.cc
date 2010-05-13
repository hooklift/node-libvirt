// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hypervisor.h"

//TODO figure out how to implement event based support

namespace NodeLibvirt {

    void Hypervisor::Initialize(Handle<Object> target) { 
        HandleScope scope;
        
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
 
        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);
        
        NODE_SET_PROTOTYPE_METHOD(t, "getBaselineCPU",
                                      Hypervisor::GetBaselineCPU);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "compareCPU",
                                      Hypervisor::CompareCPU);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getCapabilities",
                                      Hypervisor::GetCapabilities);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getHostname",
                                      Hypervisor::GetHostname);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getLibVirtVersion",
                                      Hypervisor::GetLibVirtVersion);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getMaxVcpus",
                                      Hypervisor::GetMaxVcpus);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getType",
                                      Hypervisor::GetType);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getConnectionUri",
                                      Hypervisor::GetConnectionUri);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getVersion",
                                      Hypervisor::GetVersion);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "isConnectionEncrypted",
                                      Hypervisor::IsConnectionEncrypted);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "isConnectionSecure",
                                      Hypervisor::IsConnectionSecure);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "closeConnection",
                                      Hypervisor::CloseConnection);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getDefinedDomains",
                                      Hypervisor::GetDefinedDomains);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getDefinedInterfaces",
                                      Hypervisor::GetDefinedInterfaces);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getDefinedNetworks",
                                      Hypervisor::GetDefinedNetworks);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getDefinedStoragePools",
                                      Hypervisor::GetDefinedStoragePools);
                                      
        //NODE_SET_PROTOTYPE_METHOD(t, "getActiveDomains",
        //                              Hypervisor::GetActiveDomains);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getActiveInterfaces",
                                      Hypervisor::GetActiveInterfaces);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNetworkFilters",
                                      Hypervisor::GetNetworkFilters);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getActiveNetworks",
                                      Hypervisor::GetActiveNetworks);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getSecrets",
                                      Hypervisor::GetSecrets);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getActiveStoragePools",
                                      Hypervisor::GetActiveStoragePools);
                                      
        /*NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfDefinedDomains",
                                      Hypervisor::GetNumberOfDefinedDomains);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfDefinedInterfaces",
                                      Hypervisor::GetNumberOfDefinedInterfaces);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfDefinedNetworks",
                                      Hypervisor::GetNumberOfDefinedNetworks);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfDefinedStoragePools",
                                      Hypervisor::GetNumberOfDefinedStoragePools);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfActiveDomains",
                                      Hypervisor::GetNumberOfActiveDomains);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfActiveInterfaces",
                                      Hypervisor::GetNumberOfActiveInterfaces);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfActiveNetworks",
                                      Hypervisor::GetNumberOfActiveNetworks);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfNetworkFilters",
                                      Hypervisor::GetNumberOfNetworkFilters);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfSecrets",
                                      Hypervisor::GetNumberOfSecrets);
                                      
        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfActiveStoragePools",
                                      Hypervisor::GetNumberOfActiveStoragePools);*/
                                      
        target->Set(String::NewSymbol("Hypervisor"), t->GetFunction());
    }
    
    Hypervisor::Hypervisor(const Local<String>& uriStr, bool readOnly) : EventEmitter(){
        HandleScope scope;
        String::Utf8Value uriUtf8(uriStr);
        const char *uri = ToCString(uriUtf8);

        //FIXME receive auth Object
        conn = virConnectOpenAuth(uri, virConnectAuthPtrDefault,
                                   readOnly ? VIR_CONNECT_RO : 0);

        if(conn == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
        }
    }
      
    Hypervisor::~Hypervisor(){
        assert(conn == NULL);
        
    }
    
    Handle<Value> Hypervisor::New(const Arguments& args) {
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
        
        Hypervisor *hypervisor = new Hypervisor(args[0]->ToString(), args[1]->IsTrue());
        hypervisor->Wrap(args.This());
        
        return args.This();
    }
    
    Handle<Value> Hypervisor::GetCapabilities(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_capabilities(); 
    }
    
    Handle<Value> Hypervisor::get_capabilities() {
        char *cap = virConnectGetCapabilities(conn);
        
        if(cap == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }
        
        Local<String> capabilities = String::New((const char*)cap);
        free(cap);
        
        return capabilities;
    }
    
    Handle<Value> Hypervisor::GetHostname(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        
        return hypervisor->get_hostname();
    }
    
    Handle<Value> Hypervisor::get_hostname() {
        char *hn = virConnectGetHostname(conn);
        
        if(hn == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }
        
        Local<String> hostname = String::New((const char*)hn);
        free(hn);
        
        return hostname;
    }
    
    Handle<Value> Hypervisor::CloseConnection(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        return hypervisor->close_connection();
    }
    
    Handle<Value> Hypervisor::close_connection() {
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
    
    Handle<Value> Hypervisor::GetLibVirtVersion(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        return hypervisor->get_libvirt_version();
    }
    
    Handle<Value> Hypervisor::get_libvirt_version() {
        unsigned long *libVer;
        unsigned int major;
        unsigned int minor;
        unsigned int rel;
        
        libVer = new unsigned long;
        
        int ret = virConnectGetLibVersion(conn, libVer);
        
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }
        
        major = *libVer / 1000000;
        *libVer %= 1000000;
        minor = *libVer / 1000;
        rel = *libVer % 1000;
        delete libVer;
             
        char vrs[10];
        sprintf(vrs, "%d.%d.%d", major, minor, rel);
        
        Local<String> version = String::New(vrs);
        
        return version;
    }
    
    
    Handle<Value> Hypervisor::GetType(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        return hypervisor->get_type();
    }
    
    Handle<Value> Hypervisor::get_type() {
        const char *t = virConnectGetType(conn);
        
        if(t == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }
        
        Local<String> type = String::New(t);
        
        return type;
    }
    
    Handle<Value> Hypervisor::GetMaxVcpus(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        return hypervisor->get_max_vcpus();
    }
    
    Handle<Value> Hypervisor::get_max_vcpus() {
        const char *type = virConnectGetType(conn);
        
        if(type == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }
        
        int m = virConnectGetMaxVcpus(conn, type);
        
        if(m == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }
        
        Local<Number> max = Number::New(m);
        
        return max;
    }
    
    Handle<Value> Hypervisor::GetConnectionUri(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        return hypervisor->get_connection_uri();
    }
    
    Handle<Value> Hypervisor::get_connection_uri() {
        char *u = virConnectGetURI(conn);
        
        if(u == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }
         
        Local<String> uri = String::New(u);
        free(u);
        
        return uri;
    }
    
    Handle<Value> Hypervisor::GetVersion(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        return hypervisor->get_version();
    }
    
    Handle<Value> Hypervisor::get_version() {
        unsigned long *hvVer;
        unsigned int major;
        unsigned int minor;
        unsigned int rel;
        
        hvVer = new unsigned long;
        
        int ret = virConnectGetVersion(conn, hvVer);
        
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            delete hvVer;
            return Null();
        }
        
        if(ret == 0 && *hvVer == 0) {
            delete hvVer;
            return Null(); 
        }
        
        major = *hvVer / 1000000;
        *hvVer %= 1000000;
        minor = *hvVer / 1000;
        rel = *hvVer % 1000;
        delete hvVer;
             
        char vrs[10];
        sprintf(vrs, "%d.%d.%d", major, minor, rel);
        
        Local<String> version = String::New(vrs);
   
        return version;
    }
    
    Handle<Value> Hypervisor::IsConnectionEncrypted(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        return hypervisor->is_connection_encrypted();
    }
    
    Handle<Value> Hypervisor::is_connection_encrypted() {
        int ret = virConnectIsEncrypted(conn);
        
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
        }
        
        if(ret == 1) {
            return True();
        }

       return False();
    }
    
    Handle<Value> Hypervisor::IsConnectionSecure(const Arguments& args) {
        HandleScope scope;
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        return hypervisor->is_connection_secure();
    }
    
    Handle<Value> Hypervisor::is_connection_secure() {
        int ret = virConnectIsSecure(conn);
        
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
        }
        
        if(ret == 1) {
            return True();
        }

       return False();
    }
    
     Handle<Value> Hypervisor::GetBaselineCPU(const Arguments& args) {
        HandleScope scope;
        
        const char **xmlCPUs = NULL;
        unsigned int ncpus = 0;
        unsigned int flags = 0;
    
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
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        
        return hypervisor->get_baseline_cpu(xmls1, ncpus, flags); 
    }
    
    Handle<Value> Hypervisor::get_baseline_cpu( char **xmlCPUs, 
                                                unsigned int ncpus, 
                                                unsigned int flags) {  
        const char *x = virConnectBaselineCPU(conn, (const char**)xmlCPUs, ncpus, flags);
        
        for (int i = 0; i < ncpus; i++) {
            free(xmlCPUs[i]);
        }
         
        delete [] xmlCPUs;
                
        if(x == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }
        
        Local<String> xml = String::New(x);            
        return xml;
    }
    
    Handle<Value> Hypervisor::CompareCPU(const Arguments& args) {
        HandleScope scope;
        
        char *xmlDesc = NULL;
        unsigned int flags = 0;
        
        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You need to specify a string as first argument")));    
        }
        
        String::Utf8Value cpu(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());        

        return hypervisor->compare_cpu(ToCString(cpu), flags); 
    }
    
    Handle<Value> Hypervisor::compare_cpu( const char *xmlDesc, unsigned int flags) {
        int ret = virConnectCompareCPU(conn, xmlDesc, flags);
                
        if(ret == VIR_CPU_COMPARE_ERROR) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }
        Local<Number> result = Number::New(ret);
        
        return result;
    }

    Handle<Value> Hypervisor::GetDefinedDomains(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_defined_domains();
    }

    Handle<Value> Hypervisor::get_defined_domains() {
        GET_LIST_OF(virConnectNumOfDefinedDomains, 
                    virConnectListDefinedDomains);
    }
    
    Handle<Value> Hypervisor::GetDefinedInterfaces(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_defined_interfaces();
    }
    
    Handle<Value> Hypervisor::get_defined_interfaces() {
        GET_LIST_OF(virConnectNumOfDefinedInterfaces, 
                    virConnectListDefinedInterfaces);
    }
    
    Handle<Value> Hypervisor::GetDefinedNetworks(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_defined_networks();
    }
    
    Handle<Value> Hypervisor::get_defined_networks() {
        GET_LIST_OF(virConnectNumOfDefinedNetworks, 
                    virConnectListDefinedNetworks);
    }
    
    Handle<Value> Hypervisor::GetDefinedStoragePools(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_defined_storage_pools();
    }

    Handle<Value> Hypervisor::get_defined_storage_pools() {
        GET_LIST_OF(virConnectNumOfDefinedStoragePools, 
                    virConnectListDefinedStoragePools);
    }
    
    /*Handle<Value> Hypervisor::GetActiveDomains(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_active_domains();
    }

    Handle<Value> Hypervisor::get_active_domains() {
        GET_LIST_OF(virConnectNumOfDomains, 
                    virConnectListDomains);
    }*/
    
    Handle<Value> Hypervisor::GetActiveInterfaces(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_active_interfaces();
    }

    Handle<Value> Hypervisor::get_active_interfaces() {
        GET_LIST_OF(virConnectNumOfInterfaces, 
                    virConnectListInterfaces);
    }
    
    Handle<Value> Hypervisor::GetNetworkFilters(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_network_filters();
    }

    Handle<Value> Hypervisor::get_network_filters() {
        GET_LIST_OF(virConnectNumOfNWFilters, 
                    virConnectListNWFilters);
    }
    
    Handle<Value> Hypervisor::GetActiveNetworks(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_active_networks();
    }

    Handle<Value> Hypervisor::get_active_networks() {
        GET_LIST_OF(virConnectNumOfNetworks, 
                    virConnectListNetworks);
    }
    
    Handle<Value> Hypervisor::GetSecrets(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_secrets();
    }

    Handle<Value> Hypervisor::get_secrets() {
        GET_LIST_OF(virConnectNumOfSecrets, 
                    virConnectListSecrets);
    }
    
    Handle<Value> Hypervisor::GetActiveStoragePools(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_active_storage_pools();
    }

    Handle<Value> Hypervisor::get_active_storage_pools() {
        GET_LIST_OF(virConnectNumOfStoragePools, 
                    virConnectListStoragePools);
    }

    
} //namespace NodeLibvirt
