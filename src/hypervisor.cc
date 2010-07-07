// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hypervisor.h"

#define GET_LIST_OF(name, numof_function, list_function)                \
                                                                        \
Handle<Value> Hypervisor::name(const Arguments& args) {                 \
    HandleScope scope;                                                  \
    char **_names = NULL;                                               \
    int numInactiveItems;                                               \
    virConnectPtr connection;                                           \
                                                                        \
    Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());                                                                    \
    connection = hypervisor->conn_;                                     \
                                                                        \
    numInactiveItems = numof_function(connection);                      \
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
    int ret = list_function(connection, _names, numInactiveItems);\
                                                                        \
    if(ret == -1) {                                                     \
        ThrowException(Error::New(virGetLastError()));                  \
        free(_names);                                                   \
        return Null();                                                  \
    }                                                                   \
                                                                        \
    TO_V8_ARRAY(numInactiveItems, _names);                              \
}


#define GET_NUM_OF(name, function)                                          \
                                                                            \
Handle<Value> Hypervisor::name(const Arguments& args) {                     \
    HandleScope scope;                                                      \
    Hypervisor *hypervisor =                                                \
    ObjectWrap::Unwrap<Hypervisor>(args.This());                            \
                                                                            \
    int ret = function(hypervisor->conn_);                                  \
    if(ret == -1) {                                                         \
        ThrowException(Error::New(virGetLastError()));                      \
        return Null();                                                      \
    }                                                                       \
    return scope.Close(Integer::New(ret));                                  \
}                                                                           \

namespace NodeLibvirt {

    static Persistent<String> node_info_model_symbol;
    static Persistent<String> node_info_memory_symbol;
    static Persistent<String> node_info_cpus_symbol;
    static Persistent<String> node_info_mhz_symbol;
    static Persistent<String> node_info_nodes_symbol;
    static Persistent<String> node_info_sockets_symbol;
    static Persistent<String> node_info_cores_symbol;
    static Persistent<String> node_info_threads_symbol;

    static Persistent<String> security_model_symbol;
    static Persistent<String> security_model_doi_symbol;


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

        NODE_SET_PROTOTYPE_METHOD(t, "getNodeFreeMemoryInNumaCells",
                                      Hypervisor::GetNodeFreeMemoryInNumaCells);

        NODE_SET_PROTOTYPE_METHOD(t, "getNodeFreeMemory",
                                      Hypervisor::GetNodeFreeMemory);

        NODE_SET_PROTOTYPE_METHOD(t, "getNodeInfo",
                                      Hypervisor::GetNodeInfo);

        NODE_SET_PROTOTYPE_METHOD(t, "getNodeDevicesNames",
                                      Hypervisor::GetNodeDevicesNames);

        NODE_SET_PROTOTYPE_METHOD(t, "getNodeSecurityModel",
                                      Hypervisor::GetNodeSecurityModel);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupNodeDeviceByName",
                                      NodeDevice::LookupByName);

        NODE_SET_PROTOTYPE_METHOD(t, "createNodeDevice",
                                      NodeDevice::Create);

        NODE_SET_PROTOTYPE_METHOD(t, "createNetwork",
                                      Network::Create);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkByName",
                                      Network::LookupByName);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkByUUID",
                                      Network::LookupByUUID);

        NODE_SET_PROTOTYPE_METHOD(t, "defineNetwork",
                                      Network::Define);

        NODE_SET_PROTOTYPE_METHOD(t, "defineNetworkFilter",
                                      NetworkFilter::Define);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkFilterByName",
                                      NetworkFilter::LookupByName);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkFilterByUUID",
                                      NetworkFilter::LookupByUUID);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupInterfaceByName",
                                      Interface::LookupByName);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupInterfaceByMacAddress",
                                      Interface::LookupByMacAddress);

        NODE_SET_PROTOTYPE_METHOD(t, "defineInterface",
                                      Interface::Define);

        NODE_SET_PROTOTYPE_METHOD(t, "defineSecret",
                                      Secret::Define);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupSecretByUsage",
                                      Secret::LookupByUsage);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupSecretByUUID",
                                      Secret::LookupByUUID);

        NODE_SET_PROTOTYPE_METHOD(t, "createStoragePool",
                                      StoragePool::Create);

        NODE_SET_PROTOTYPE_METHOD(t, "defineStoragePool",
                                      StoragePool::Define);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupStoragePoolByName",
                                      StoragePool::LookupByName);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupStoragePoolByUUID",
                                      StoragePool::LookupByUUID);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupStoragePoolByVolume",
                                      StoragePool::LookupByVolume);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupStorageVolumeByKey",
                                      StorageVolume::LookupByKey);

        NODE_SET_PROTOTYPE_METHOD(t, "lookupStorageVolumeByPath",
                                      StorageVolume::LookupByPath);

        Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

        //Constants initialization
        //virConnectCredentialType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_USERNAME);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_AUTHNAME);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_LANGUAGE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_CNONCE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_PASSPHRASE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_ECHOPROMPT);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_NOECHOPROMPT);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_REALM);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_EXTERNAL);

        //virCPUCompareResult
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CPU_COMPARE_ERROR);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CPU_COMPARE_INCOMPATIBLE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CPU_COMPARE_IDENTICAL);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_CPU_COMPARE_SUPERSET);

        //virSecretUsageType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_SECRET_USAGE_TYPE_NONE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_SECRET_USAGE_TYPE_VOLUME);

        node_info_model_symbol      = NODE_PSYMBOL("model");
        node_info_memory_symbol     = NODE_PSYMBOL("memory");
        node_info_cpus_symbol       = NODE_PSYMBOL("cpus");
        node_info_mhz_symbol        = NODE_PSYMBOL("mhz");
        node_info_nodes_symbol      = NODE_PSYMBOL("nodes");
        node_info_sockets_symbol    = NODE_PSYMBOL("sockets");
        node_info_cores_symbol      = NODE_PSYMBOL("cores");
        node_info_threads_symbol    = NODE_PSYMBOL("threads");

        security_model_symbol       = NODE_PSYMBOL("model");
        security_model_doi_symbol   = NODE_PSYMBOL("doi");

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
        Local<Object> obj = args.This();
        hypervisor->Wrap(obj);

        return obj;
    }

    Handle<Value> Hypervisor::GetCapabilities(const Arguments& args) {
        HandleScope scope;
        char* capabilities_ = NULL;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        capabilities_ = virConnectGetCapabilities(hypervisor->conn_);

        if(capabilities_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> capabilities = String::New((const char*)capabilities_);
        free(capabilities_);

        return scope.Close(capabilities);
    }

    Handle<Value> Hypervisor::GetHostname(const Arguments& args) {
        HandleScope scope;
        char* hostname_ = NULL;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        hostname_ = virConnectGetHostname(hypervisor->conn_);

        if(hostname_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> hostname = String::New((const char*)hostname_);
        free(hostname_);

        return scope.Close(hostname);
    }

    Handle<Value> Hypervisor::CloseConnection(const Arguments& args) {
        HandleScope scope;
        int is_closed = -1;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        if(hypervisor->conn_ != NULL) {
            is_closed = virConnectClose(hypervisor->conn_);

            if(is_closed == -1) {
                return False();
            }
        }
        hypervisor->conn_ = NULL;
        return True();
    }

    Handle<Value> Hypervisor::GetLibVirtVersion(const Arguments& args) {
        HandleScope scope;
        unsigned long *libVer = NULL;
        unsigned int major = 0;
        unsigned int minor = 0;
        unsigned int rel = 0;
        int ret = -1;

        libVer = new unsigned long;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        ret = virConnectGetLibVersion(hypervisor->conn_, libVer);

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

        return scope.Close(version);
    }

    Handle<Value> Hypervisor::GetType(const Arguments& args) {
        HandleScope scope;
        const char* type_ = NULL;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        type_ = virConnectGetType(hypervisor->conn_);

        if(type_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> type = String::New(type_);

        return scope.Close(type);
    }

    Handle<Value> Hypervisor::GetMaxVcpus(const Arguments& args) {
        HandleScope scope;
        const char *type = NULL;
        int maxvcpus_ = 0;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        type = virConnectGetType(hypervisor->conn_);

        if(type == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        maxvcpus_ = virConnectGetMaxVcpus(hypervisor->conn_, type);

        if(maxvcpus_ == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Number> maxvcpus = Number::New(maxvcpus_);

        return scope.Close(maxvcpus);
    }

    Handle<Value> Hypervisor::GetConnectionUri(const Arguments& args) {
        HandleScope scope;
        char *uri_ = NULL;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        uri_ = virConnectGetURI(hypervisor->conn_);

        if(uri_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> uri = String::New(uri_);
        free(uri_);

        return scope.Close(uri);
    }

    Handle<Value> Hypervisor::GetVersion(const Arguments& args) {
        HandleScope scope;
        unsigned long *hvVer = NULL;
        unsigned int major = 0;
        unsigned int minor = 0;
        unsigned int rel = 0;
        int ret = -1;

        hvVer = new unsigned long;
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        ret = virConnectGetVersion(hypervisor->conn_, hvVer);

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

        char version_[10];
        sprintf(version_, "%d.%d.%d", major, minor, rel);

        Local<String> version = String::New(version_);

        return scope.Close(version);
    }

    Handle<Value> Hypervisor::IsConnectionEncrypted(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        ret = virConnectIsEncrypted(hypervisor->conn_);

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
        int ret = -1;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        ret = virConnectIsSecure(hypervisor->conn_);

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
        char **xmlCPUs = NULL;
        unsigned int ncpus = 0;
        unsigned int flags = 0;

        if(args.Length() == 0 || !args[0]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an Array with two xml's to compute the most feature-rich CPU")));
        }

        Local<Array> xmls = Local<Array>::Cast(args[0]);
        ncpus = xmls->Length();
        xmlCPUs = new char*[ncpus + 1];
        xmlCPUs[ncpus] = NULL;
        for (int i = 0; i < ncpus; i++) {
            String::Utf8Value cpu(xmls->Get(Integer::New(i))->ToString());
            xmlCPUs[i] = strdup(*cpu);
        }
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        const char *x = virConnectBaselineCPU(hypervisor->conn_, (const char**)xmlCPUs, ncpus, flags);

        for (int i = 0; i < ncpus; i++) {
            free(xmlCPUs[i]);
        }

        delete [] xmlCPUs;

        if(x == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> xml = String::New(x);
        return scope.Close(xml);
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

        int ret = virConnectCompareCPU(hypervisor->conn_, ToCString(cpu), flags);

        if(ret == VIR_CPU_COMPARE_ERROR) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        Local<Number> result = Number::New(ret);

        return scope.Close(result);
    }

    Handle<Value> Hypervisor::GetActiveDomains(const Arguments& args) {
        HandleScope scope;
        int *ids = NULL;
        int numOfDomains = 0;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        numOfDomains = virConnectNumOfDomains(hypervisor->conn_);

        if(numOfDomains == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        ids = new int[numOfDomains];
        if(ids == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return Null();
        }

        int ret = virConnectListDomains(hypervisor->conn_, ids, numOfDomains);

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
        return scope.Close(v8Array);
    }

    Handle<Value> Hypervisor::GetNodeFreeMemoryInNumaCells(const Arguments& args) {
        HandleScope scope;
        int start_cell = 0;
        int max_cells = 0;

        unsigned long long *free_memory = NULL;

        if(args.Length() < 2) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify two arguments to call this function")));
        }

        if(!args[0]->IsInt32() || !args[1]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("The two arguments must be numeric")));
        }

        start_cell = args[0]->Int32Value();
        max_cells = args[1]->Int32Value();

        if ((start_cell < 0) || (max_cells <= 0) || (start_cell + max_cells > 10000)) {
            LIBVIRT_THROW_EXCEPTION("Inconsistent cell bounds");
            return Null();
        }

        free_memory = (unsigned long long*) malloc(max_cells * sizeof(*free_memory));
        if(free_memory == NULL) {
            LIBVIRT_THROW_EXCEPTION("Unable to allocate memory");
            return Null();
        }

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        int cells_ = virNodeGetCellsFreeMemory(hypervisor->conn_, free_memory, start_cell, max_cells);

        if(cells_ == -1) {
            free(free_memory);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Array> cells = Array::New(cells_);
        for(int i = 0; i < cells_; i++) {
            cells->Set(Integer::New(i), Number::New(free_memory[i]));
        }

        free(free_memory);

        return scope.Close(cells);
    }

    Handle<Value> Hypervisor::GetNodeFreeMemory(const Arguments& args) {
        HandleScope scope;
        unsigned long long memory = 0;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        memory = virNodeGetFreeMemory(hypervisor->conn_);

        if(memory == 0) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(Number::New(memory));
    }

    Handle<Value> Hypervisor::GetNodeInfo(const Arguments& args) {
        HandleScope scope;
        virNodeInfo info_;
        int ret = -1;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        ret = virNodeGetInfo(hypervisor->conn_, &info_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> info = Object::New();
        info->Set(node_info_model_symbol, String::New(info_.model));
        info->Set(node_info_memory_symbol, Number::New(info_.memory));
        info->Set(node_info_cpus_symbol, Integer::New(info_.cpus));
        info->Set(node_info_mhz_symbol, Integer::New(info_.mhz));
        info->Set(node_info_nodes_symbol, Integer::New(info_.nodes));
        info->Set(node_info_sockets_symbol, Integer::New(info_.sockets));
        info->Set(node_info_cores_symbol, Integer::New(info_.cores));
        info->Set(node_info_threads_symbol, Integer::New(info_.threads));

        return scope.Close(info);
    }

    Handle<Value> Hypervisor::GetNodeDevicesNames(const Arguments& args) {
        HandleScope scope;
        const char *cap = NULL;
        unsigned int flags = 0;
        char **names = NULL;
        int num_devices = -1;

        if(args.Length() == 1 && !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument")));
        }

        String::Utf8Value cap_(args[0]->ToString());
        cap = ToCString(cap_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        num_devices = virNodeNumOfDevices(hypervisor->conn_, cap, flags);

        if(num_devices == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        names = (char **) malloc(sizeof(*names) * num_devices);
        if(names == NULL) {
            LIBVIRT_THROW_EXCEPTION("Unable to allocate memory");
            return Null();
        }

        num_devices = virNodeListDevices(hypervisor->conn_, cap, names, num_devices, flags);
        if(num_devices == -1) {
            free(names);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Array> devices = Array::New(num_devices);
        for(int i = 0; i < num_devices; i++) {
            devices->Set(Integer::New(i), String::New(names[num_devices]));
            free(names[num_devices]);
        }
        free(names);

        return scope.Close(devices);
    }

    Handle<Value> Hypervisor::GetNodeSecurityModel(const Arguments& args) {
        HandleScope scope;
        virSecurityModel secmodel;
        int ret = -1;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        ret = virNodeGetSecurityModel(hypervisor->conn_, &secmodel);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> object = Object::New();
        object->Set(security_model_symbol, String::New(secmodel.model));
        object->Set(security_model_doi_symbol, String::New(secmodel.doi));

        return scope.Close(object);
    }

    GET_LIST_OF( GetDefinedDomains,
                 virConnectNumOfDefinedDomains,
                 virConnectListDefinedDomains);

    GET_LIST_OF( GetDefinedInterfaces,
                 virConnectNumOfDefinedInterfaces,
                 virConnectListDefinedInterfaces);

    GET_LIST_OF( GetDefinedNetworks,
                 virConnectNumOfDefinedNetworks,
                 virConnectListDefinedNetworks);

    GET_LIST_OF( GetDefinedStoragePools,
                 virConnectNumOfDefinedStoragePools,
                 virConnectListDefinedStoragePools);

    GET_LIST_OF( GetActiveInterfaces,
                 virConnectNumOfInterfaces,
                 virConnectListInterfaces);

    GET_LIST_OF( GetNetworkFilters,
                 virConnectNumOfNWFilters,
                 virConnectListNWFilters);

    GET_LIST_OF( GetActiveNetworks,
                 virConnectNumOfNetworks,
                 virConnectListNetworks);

    GET_LIST_OF( GetSecrets,
                 virConnectNumOfSecrets,
                 virConnectListSecrets);

    GET_LIST_OF( GetActiveStoragePools,
                 virConnectNumOfStoragePools,
                 virConnectListStoragePools);

    GET_NUM_OF(GetNumberOfDefinedDomains, virConnectNumOfDefinedDomains);

    GET_NUM_OF(GetNumberOfDefinedInterfaces, virConnectNumOfDefinedInterfaces);

    GET_NUM_OF(GetNumberOfDefinedNetworks, virConnectNumOfDefinedNetworks);

    GET_NUM_OF(GetNumberOfDefinedStoragePools, virConnectNumOfDefinedStoragePools);

    GET_NUM_OF(GetNumberOfActiveDomains, virConnectNumOfDomains);

    GET_NUM_OF(GetNumberOfActiveInterfaces, virConnectNumOfInterfaces);

    GET_NUM_OF(GetNumberOfActiveNetworks, virConnectNumOfNetworks);

    GET_NUM_OF(GetNumberOfNetworkFilters, virConnectNumOfNWFilters);

    GET_NUM_OF(GetNumberOfSecrets, virConnectNumOfSecrets);

    GET_NUM_OF(GetNumberOfActiveStoragePools, virConnectNumOfStoragePools);

} //namespace NodeLibvirt

