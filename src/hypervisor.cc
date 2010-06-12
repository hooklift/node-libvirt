// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hypervisor.h"

#define GET_LIST_OF(numof_function, list_function)                      \
({                                                                      \
                                                                        \
    char **_names = NULL;                                               \
    int numInactiveItems;                                               \
                                                                        \
    numInactiveItems = numof_function(conn_);                           \
                                                                        \
    if(numInactiveItems == -1) {                                        \
        ThrowException(Error::New(virGetLastError()));                  \
        return Null();                                                  \
    }                                                                   \
                                                                        \
    _names = (char **)malloc(sizeof(*_names) * numInactiveItems);       \
    if(_names == NULL) {                                                \
        LIBVIRT_THROW_EXCEPTION("unable to allocate memory");           \
        return Null();                                                  \
    }                                                                   \
                                                                        \
    int ret = list_function(conn_, _names, numInactiveItems);           \
                                                                        \
    if(ret == -1) {                                                     \
        ThrowException(Error::New(virGetLastError()));                  \
        free(_names);                                                   \
        return Null();                                                  \
    }                                                                   \
                                                                        \
    TO_V8_ARRAY(numInactiveItems, _names);                              \
})

#define GET_NUM_OF(function)                                            \
({                                                                      \
    int ret = function(conn_);                                          \
    if(ret == -1) {                                                     \
        ThrowException(Error::New(virGetLastError()));                  \
        return Null();                                                  \
    }                                                                   \
    return Integer::New(ret);                                           \
})

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

        NODE_SET_PROTOTYPE_METHOD(t, "getActiveDomains",
                                      Hypervisor::GetActiveDomains);

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

        NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfDefinedDomains",
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
                                      Hypervisor::GetNumberOfActiveStoragePools);

        NODE_SET_PROTOTYPE_METHOD(t, "createDomain",
                                      Domain::Create);

        NODE_SET_PROTOTYPE_METHOD(t, "defineDomain",
                                      Domain::Define);

        NODE_SET_PROTOTYPE_METHOD(t, "restoreDomain",
                                      Domain::Restore);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupDomainById",
                                      Domain::LookupById);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupDomainByName",
                                      Domain::LookupByName);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupDomainByUUID",
                                      Domain::LookupByUUID);

        t->SetClassName(String::NewSymbol("Hypervisor"));
        target->Set(String::NewSymbol("Hypervisor"), t->GetFunction());
    }

    Hypervisor::Hypervisor(const Local<String>& uriStr, bool readOnly) : EventEmitter() {
        HandleScope scope;
        String::Utf8Value uriUtf8(uriStr);
        const char *uri = ToCString(uriUtf8);

        //FIXME receive auth Object
        conn_ = virConnectOpenAuth(uri, virConnectAuthPtrDefault,
                                   readOnly ? VIR_CONNECT_RO : 0);

        if(conn_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
        }
    }

    Hypervisor::~Hypervisor(){
        assert(conn_ == NULL);
    }

    virConnectPtr Hypervisor::connection() const {
        return conn_;
    }

    Handle<Value> Hypervisor::New(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 ) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at least a Hypervisor URI")));
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
        //hypervisor->openConnection(args) google code style, no work in constructors
        Local<Object> obj = args.This();

        //Constants initialization
        //virConnectCredentialType
        NODE_DEFINE_CONSTANT(obj, VIR_CRED_USERNAME);
        NODE_DEFINE_CONSTANT(obj, VIR_CRED_AUTHNAME);
        NODE_DEFINE_CONSTANT(obj, VIR_CRED_LANGUAGE);
        NODE_DEFINE_CONSTANT(obj, VIR_CRED_CNONCE);
        NODE_DEFINE_CONSTANT(obj, VIR_CRED_PASSPHRASE);
        NODE_DEFINE_CONSTANT(obj, VIR_CRED_ECHOPROMPT);
        NODE_DEFINE_CONSTANT(obj, VIR_CRED_NOECHOPROMPT);
        NODE_DEFINE_CONSTANT(obj, VIR_CRED_REALM);
        NODE_DEFINE_CONSTANT(obj, VIR_CRED_EXTERNAL);

        //virCPUCompareResult
        NODE_DEFINE_CONSTANT(obj, VIR_CPU_COMPARE_ERROR);
        NODE_DEFINE_CONSTANT(obj, VIR_CPU_COMPARE_INCOMPATIBLE);
        NODE_DEFINE_CONSTANT(obj, VIR_CPU_COMPARE_IDENTICAL);
        NODE_DEFINE_CONSTANT(obj, VIR_CPU_COMPARE_SUPERSET);

        hypervisor->Wrap(obj);

        return obj;
    }

    Handle<Value> Hypervisor::GetCapabilities(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_capabilities();
    }

    Handle<Value> Hypervisor::get_capabilities() {
        char *cap = virConnectGetCapabilities(conn_);

        if(cap == NULL) {
            ThrowException(Error::New(virGetLastError()));
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
        char *hn = virConnectGetHostname(conn_);

        if(hn == NULL) {
            ThrowException(Error::New(virGetLastError()));
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
        if(conn_ != NULL) {
            isClosed = virConnectClose(conn_);

            if(isClosed == -1) {
                return False();
            }
        }
        conn_ = NULL;
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

        int ret = virConnectGetLibVersion(conn_, libVer);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
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
        const char *t = virConnectGetType(conn_);

        if(t == NULL) {
            ThrowException(Error::New(virGetLastError()));
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
        const char *type = virConnectGetType(conn_);

        if(type == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        int m = virConnectGetMaxVcpus(conn_, type);

        if(m == -1) {
            ThrowException(Error::New(virGetLastError()));
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
        char *u = virConnectGetURI(conn_);

        if(u == NULL) {
            ThrowException(Error::New(virGetLastError()));
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

        int ret = virConnectGetVersion(conn_, hvVer);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
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
        int ret = virConnectIsEncrypted(conn_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
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
        int ret = virConnectIsSecure(conn_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
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
            String::New("You must specify an Array with two xml's to compute the most feature-rich CPU")));
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
        const char *x = virConnectBaselineCPU(conn_, (const char**)xmlCPUs, ncpus, flags);

        for (int i = 0; i < ncpus; i++) {
            free(xmlCPUs[i]);
        }

        delete [] xmlCPUs;

        if(x == NULL) {
            ThrowException(Error::New(virGetLastError()));
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
            String::New("You must specify a string as first argument")));
        }

        String::Utf8Value cpu(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->compare_cpu(ToCString(cpu), flags);
    }

    Handle<Value> Hypervisor::compare_cpu( const char *xmlDesc, unsigned int flags) {
        int ret = virConnectCompareCPU(conn_, xmlDesc, flags);

        if(ret == VIR_CPU_COMPARE_ERROR) {
            ThrowException(Error::New(virGetLastError()));
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

    Handle<Value> Hypervisor::GetActiveDomains(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_active_domains();
    }

    Handle<Value> Hypervisor::get_active_domains() {
        int *ids = NULL;
        int numOfDomains;

        numOfDomains = virConnectNumOfDomains(conn_);

        if(numOfDomains == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        ids = new int[numOfDomains];
        if(ids == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return Null();
        }

        int ret = virConnectListDomains(conn_, ids, numOfDomains);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            delete [] ids;
            return Null();
        }

        Local<Array> v8Array = Array::New(numOfDomains);
        for(int i = 0; i < numOfDomains; i++) {
            v8Array->Set(Integer::New(i), Integer::New(ids[i]));
            //free(ids[i]);
        }
        delete [] ids;
        return v8Array;
    }

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

    Handle<Value> Hypervisor::GetNumberOfDefinedDomains(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_defined_domains();
    }

    Handle<Value> Hypervisor::get_number_of_defined_domains() {
        GET_NUM_OF(virConnectNumOfDefinedDomains);
    }

    Handle<Value> Hypervisor::GetNumberOfDefinedInterfaces(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_defined_interfaces();
    }

    Handle<Value> Hypervisor::get_number_of_defined_interfaces() {
        GET_NUM_OF(virConnectNumOfDefinedInterfaces);
    }

    Handle<Value> Hypervisor::GetNumberOfDefinedNetworks(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_defined_networks();
    }

    Handle<Value> Hypervisor::get_number_of_defined_networks() {
        GET_NUM_OF(virConnectNumOfDefinedNetworks);
    }

    Handle<Value> Hypervisor::GetNumberOfDefinedStoragePools(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_defined_storage_pools();
    }

    Handle<Value> Hypervisor::get_number_of_defined_storage_pools() {
        GET_NUM_OF(virConnectNumOfDefinedStoragePools);
    }

    Handle<Value> Hypervisor::GetNumberOfActiveDomains(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_active_domains();
    }

    Handle<Value> Hypervisor::get_number_of_active_domains() {
        GET_NUM_OF(virConnectNumOfDomains);
    }

    Handle<Value> Hypervisor::GetNumberOfActiveInterfaces(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_active_interfaces();
    }

    Handle<Value> Hypervisor::get_number_of_active_interfaces() {
        GET_NUM_OF(virConnectNumOfInterfaces);
    }

    Handle<Value> Hypervisor::GetNumberOfActiveNetworks(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_active_networks();
    }

    Handle<Value> Hypervisor::get_number_of_active_networks() {
        GET_NUM_OF(virConnectNumOfNetworks);
    }

    Handle<Value> Hypervisor::GetNumberOfNetworkFilters(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_network_filters();
    }

    Handle<Value> Hypervisor::get_number_of_network_filters() {
        GET_NUM_OF(virConnectNumOfNWFilters);
    }

    Handle<Value> Hypervisor::GetNumberOfSecrets(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_secrets();
    }

    Handle<Value> Hypervisor::get_number_of_secrets() {
        GET_NUM_OF(virConnectNumOfSecrets);
    }

    Handle<Value> Hypervisor::GetNumberOfActiveStoragePools(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        return hypervisor->get_number_of_active_storage_pools();
    }

    Handle<Value> Hypervisor::get_number_of_active_storage_pools() {
        GET_NUM_OF(virConnectNumOfStoragePools);
    }

} //namespace NodeLibvirt

