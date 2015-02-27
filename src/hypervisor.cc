// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hypervisor.h"

#define GET_LIST_OF(name, numof_function, list_function)                    \
                                                                            \
NAN_METHOD(Hypervisor::name) {                                              \
    NanScope();                                                             \
                                                                            \
    Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());   \
                                                                            \
    if (args.Length() == 1 && !args[0]->IsFunction()) {                     \
        return ThrowException(Exception::TypeError(                         \
        String::New("You must specify a function as first argument")));     \
    }                                                                       \
                                                                            \
    NanCallback *callback = new NanCallback(args[0].As<Function>());        \
                                                                            \
    NanAsyncQueueWorker(                                                    \
        new GetListOfWorker(                                                \
            callback,                                                       \
            hypervisor,                                                     \
            numof_function,                                                 \
            list_function)                                                  \
    );                                                                      \
                                                                            \
    NanReturnUndefined();                                                   \
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

    static Persistent<String> hypervisor_username_symbol;
    static Persistent<String> hypervisor_password_symbol;
    static Persistent<String> hypervisor_readonly_symbol;

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

    static Persistent<String> domain_event_callback_symbol;
    static Persistent<String> domain_event_type_symbol;
    static Persistent<String> domain_event_symbol;
    static Persistent<String> domain_event_detail_symbol;
    static Persistent<String> domain_event_hypervisor_symbol;
    static Persistent<String> domain_event_rtc_utcoffset_symbol;
    static Persistent<String> domain_event_action_symbol;
    static Persistent<String> domain_event_ioerror_srcpath_symbol;
    static Persistent<String> domain_event_ioerror_devalias_symbol;
    static Persistent<String> domain_event_ioerror_reason_symbol;
    static Persistent<String> domain_event_graphics_family_sym;
    static Persistent<String> domain_event_graphics_node_sym;
    static Persistent<String> domain_event_graphics_service_sym;
    static Persistent<String> domain_event_graphics_subjname_sym;
    static Persistent<String> domain_event_graphics_subjtype_sym;
    static Persistent<String> domain_event_graphics_local_sym;
    static Persistent<String> domain_event_graphics_remote_sym;
    static Persistent<String> domain_event_graphics_subject_sym;
    static Persistent<String> domain_event_graphics_phase_sym;
    static Persistent<String> domain_event_graphics_authscheme_sym;


    void Hypervisor::Initialize(Handle<Object> target) {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);

//        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "connect",
                                      Hypervisor::Connect);

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

        NODE_SET_PROTOTYPE_METHOD(t, "getSysinfo",
                                      Hypervisor::GetSysinfo);

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

        NODE_SET_PROTOTYPE_METHOD(t, "isConnectionAlive",
                                      Hypervisor::IsConnectionAlive);

        NODE_SET_PROTOTYPE_METHOD(t, "closeConnection",
                                      Hypervisor::CloseConnection);

        NODE_SET_PROTOTYPE_METHOD(t, "setKeepAlive",
                                      Hypervisor::SetKeepAlive);

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

        NODE_SET_PROTOTYPE_METHOD(t, "registerDomainEvent",
                                      Hypervisor::RegisterDomainEvent);

        NODE_SET_PROTOTYPE_METHOD(t, "unregisterDomainEvent",
                                      Hypervisor::UnregisterDomainEvent);

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

        //virDomainEventID
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_LIFECYCLE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_REBOOT);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_RTC_CHANGE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_WATCHDOG);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_IO_ERROR);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_GRAPHICS);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_IO_ERROR_REASON);
  #ifdef VIR_ENUM_SENTINELS
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_LAST);
  #endif

        hypervisor_username_symbol  = NODE_PSYMBOL("username");
        hypervisor_password_symbol  = NODE_PSYMBOL("password");
        hypervisor_readonly_symbol  = NODE_PSYMBOL("readonly");

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

        domain_event_callback_symbol         = NODE_PSYMBOL("callback");
        domain_event_type_symbol             = NODE_PSYMBOL("evtype");
        domain_event_detail_symbol           = NODE_PSYMBOL("detail");
        domain_event_symbol                  = NODE_PSYMBOL("domain");
        domain_event_hypervisor_symbol       = NODE_PSYMBOL("hypervisor");
        domain_event_rtc_utcoffset_symbol    = NODE_PSYMBOL("utc_offset");
        domain_event_action_symbol           = NODE_PSYMBOL("action");
        domain_event_ioerror_srcpath_symbol  = NODE_PSYMBOL("src_path");
        domain_event_ioerror_devalias_symbol = NODE_PSYMBOL("dev_alias");
        domain_event_ioerror_reason_symbol   = NODE_PSYMBOL("reason");

        domain_event_graphics_family_sym  = NODE_PSYMBOL("family");
        domain_event_graphics_node_sym    = NODE_PSYMBOL("node");
        domain_event_graphics_service_sym = NODE_PSYMBOL("service");
        domain_event_graphics_subjname_sym      = NODE_PSYMBOL("name");
        domain_event_graphics_subjtype_sym      = NODE_PSYMBOL("type");

        domain_event_graphics_local_sym         = NODE_PSYMBOL("local");
        domain_event_graphics_remote_sym        = NODE_PSYMBOL("remote");
        domain_event_graphics_subject_sym       = NODE_PSYMBOL("subject");
        domain_event_graphics_phase_sym         = NODE_PSYMBOL("phase");
        domain_event_graphics_authscheme_sym    = NODE_PSYMBOL("auth_scheme");

        t->SetClassName(String::NewSymbol("Hypervisor"));
        target->Set(String::NewSymbol("Hypervisor"), t->GetFunction());
    }


    Hypervisor::Hypervisor( char* uri,
                            char* username,
                            char* password,
                            bool readonly)
    : ObjectWrap() {

        this->uri_ = uri;
        this->username_ = username;
        this->password_ = password;
        this->readOnly_ = readonly;

    }

    int Hypervisor::auth_callback(  virConnectCredentialPtr cred,
                                    unsigned int ncred,
                                    void *data) {
        Hypervisor *hyp = static_cast<Hypervisor*>(data);

        for (unsigned int i = 0; i < ncred; i++) {
            if (cred[i].type == VIR_CRED_AUTHNAME) {
                cred[i].result = hyp->username_;

                if (cred[i].result == NULL) {
                    return -1;
                }
                cred[i].resultlen = strlen(cred[i].result);
            } else if (cred[i].type == VIR_CRED_PASSPHRASE) {
                cred[i].result = hyp->password_;
                if (cred[i].result == NULL) {
                    return -1;
                }
                cred[i].resultlen = strlen(cred[i].result);
            }
        }

        return 0;
    }

    virConnectPtr Hypervisor::connection() const {
        return conn_;
    }

    Handle<Value> Hypervisor::New(const Arguments& args) {
        HandleScope scope;

        bool readonly = false;
        char* uri = NULL;
        char* username = NULL;
        char* password = NULL;

        int argsLen = args.Length();

        if (argsLen == 0) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a connection URI as first argument")));
        }

        if (argsLen == 1 && !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as connection URI")));
        }

        Local<String> uriStr = args[0]->ToString();
        String::Utf8Value uri_(uriStr);
        uri = strdup(*uri_);

        if (argsLen >= 2) {
            if (args[1]->IsBoolean()) {
                readonly = args[1]->IsTrue();
            } else if (args[1]->IsObject()) {
                Local<Object> args_ = args[1]->ToObject();

                if (args_->Has(hypervisor_readonly_symbol) &&
                    args_->Get(hypervisor_readonly_symbol)->IsBoolean()) {
                    readonly = args_->Get(hypervisor_readonly_symbol)->IsTrue();
                }

                if (args_->Has(hypervisor_username_symbol)) {
                    String::Utf8Value username_(args_->Get(hypervisor_username_symbol));
                    //We need to use strdup because the driver
                    //is attempting to release cred[i].result memory for us.
                    username = strdup(*username_);
                }

                if (args_->Has(hypervisor_password_symbol)) {
                    String::Utf8Value password_(args_->Get(hypervisor_password_symbol));
                    //We need to use strdup because the driver
                    //is attempting to release cred[i].result memory for us.
                    password = strdup(*password_);
                }
            }
        }

        Hypervisor *hypervisor = new Hypervisor(uri, username, password, readonly);
        Local<Object> obj = args.This();
        hypervisor->Wrap(obj);

        return scope.Close(obj);
    }

    int ConnectWorker::auth_callback(  virConnectCredentialPtr cred,
                                    unsigned int ncred,
                                    void *data) {
        Hypervisor *hyp = static_cast<Hypervisor*>(data);

        for (unsigned int i = 0; i < ncred; i++) {
            if (cred[i].type == VIR_CRED_AUTHNAME) {
                cred[i].result = hyp->username_;

                if (cred[i].result == NULL) {
                    return -1;
                }
                cred[i].resultlen = strlen(cred[i].result);
            } else if (cred[i].type == VIR_CRED_PASSPHRASE) {
                cred[i].result = hyp->password_;
                if (cred[i].result == NULL) {
                    return -1;
                }
                cred[i].resultlen = strlen(cred[i].result);
            }
        }

        return 0;
    }

    void ConnectWorker::Execute() {

      static int supported_cred_types[] = {
          VIR_CRED_AUTHNAME,
          VIR_CRED_PASSPHRASE,
      };

      Hypervisor *hypervisor = this->getHypervisor();
      virConnectAuth auth;
      auth.credtype = supported_cred_types;
      auth.ncredtype = sizeof(supported_cred_types)/sizeof(int);
      auth.cb = ConnectWorker::auth_callback;
      auth.cbdata = hypervisor;

      hypervisor->conn_ = virConnectOpenAuth( (const char*) hypervisor->uri_,
                                  &auth,
                                  hypervisor->readOnly_ ? VIR_CONNECT_RO : 0);

      if(hypervisor->conn_ == NULL) {
          setVirError(virGetLastError());
      }
    }

    NAN_METHOD(Hypervisor::Connect) {
        NanScope();

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        if (args.Length() == 1 && !args[0]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as first argument")));
        }

        NanCallback *callback = new NanCallback(args[0].As<Function>());

        NanAsyncQueueWorker(new ConnectWorker(callback, hypervisor));

        NanReturnUndefined();
    }

    void GetCapabilitiesWorker::Execute() {
        Hypervisor *hypervisor = getHypervisor();

        char* capabilities_ = NULL;

        capabilities_ = virConnectGetCapabilities(hypervisor->connection());

        if(capabilities_ == NULL) {
            setVirError(virGetLastError());
            return;
        }

        setString(capabilities_);
    }

    NAN_METHOD(Hypervisor::GetCapabilities) {
        NanScope();
        
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        if (args.Length() == 1 && !args[0]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as first argument")));
        }

        NanCallback *callback = new NanCallback(args[0].As<Function>());

        NanAsyncQueueWorker(new GetCapabilitiesWorker(callback, hypervisor));

        NanReturnUndefined();
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
                ThrowException(Error::New(virGetLastError()));
                return False();
            }
        }
        hypervisor->conn_ = NULL;
        return True();
    }

    Handle<Value> Hypervisor::SetKeepAlive(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        if (args.Length() != 2) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify two integer arguments interval and count")));
        }

        if (!args[0]->IsNumber()) {
            return ThrowException(Exception::TypeError(
            String::New("Interval must be a number")));
        }

        if (!args[1]->IsNumber()) {
            return ThrowException(Exception::TypeError(
            String::New("Count must be a number")));
        }

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        ret = virConnectSetKeepAlive(hypervisor->conn_, args[0]->Int32Value(), args[1]->Int32Value());

        if (ret == -1) {
            return ThrowException(Error::New(virGetLastError()));
        }

        if (ret == 1) {
            return ThrowException(Exception::TypeError(
            String::New("Remote party doesn't support keepalive messages")));
        }

        return scope.Close(Undefined());
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

    Handle<Value> Hypervisor::GetSysinfo(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        const char* info_ = virConnectGetSysinfo(hypervisor->conn_, 0);

        if(info_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> info = String::New(info_);

        return scope.Close(info);
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
        int maxvcpus_ = 0;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        //type = virConnectGetType(hypervisor->conn_);
        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string")));
        }

        String::Utf8Value type(args[0]->ToString());

        maxvcpus_ = virConnectGetMaxVcpus(hypervisor->conn_, (const char *) *type);

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

    Handle<Value> Hypervisor::IsConnectionAlive(const Arguments& args) {
        HandleScope scope;

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        int ret = virConnectIsAlive(hypervisor->conn_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
        }

        return (ret == 1) ? True() : False();
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
        for (unsigned int i = 0; i < ncpus; i++) {
            String::Utf8Value cpu(xmls->Get(Integer::New(i))->ToString());
            xmlCPUs[i] = strdup(*cpu);
        }
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        const char *x = virConnectBaselineCPU(hypervisor->conn_, (const char**)xmlCPUs, ncpus, flags);

        for (unsigned int i = 0; i < ncpus; i++) {
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

        unsigned int flags = 0;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        String::Utf8Value cpu(args[0]->ToString());

        int ret = virConnectCompareCPU(hypervisor->conn_, (const char *) *cpu, flags);

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
        unsigned int flags = 0;
        char **names = NULL;
        int num_devices = -1;

        if(args.Length() == 1 && !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument")));
        }

        String::Utf8Value cap(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        num_devices = virNodeNumOfDevices(hypervisor->conn_, (const char *) *cap, flags);

        if(num_devices == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        names = (char **) malloc(sizeof(*names) * num_devices);
        if(names == NULL) {
            LIBVIRT_THROW_EXCEPTION("Unable to allocate memory");
            return Null();
        }

        num_devices = virNodeListDevices(hypervisor->conn_, (const char *) *cap, names, num_devices, flags);
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

    Handle<Value> Hypervisor::RegisterDomainEvent(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsObject()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a object as argument")));
        }

        Local<Object> arg_obj = args[0]->ToObject();

        if( !arg_obj->Has(domain_event_type_symbol) ||
            !arg_obj->Get(domain_event_type_symbol)->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an valid event type")));
        }

        if( !arg_obj->Has(domain_event_callback_symbol) ||
            !arg_obj->Get(domain_event_callback_symbol)->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid callback function")));
        }

        Domain *domain = NULL;

        if(arg_obj->Has(domain_event_symbol)) {
            Local<Object> domain_obj = arg_obj->Get(domain_event_symbol)->ToObject();
            if(!Domain::HasInstance(domain_obj)) {
                return ThrowException(Exception::TypeError(
                String::New("You must specify a Domain object instance")));
            }

            domain = ObjectWrap::Unwrap<Domain>(domain_obj);
        }

        virConnectDomainEventGenericCallback callback = NULL;
        int evtype = arg_obj->Get(domain_event_type_symbol)->Int32Value();

        switch (evtype) {
            case VIR_DOMAIN_EVENT_ID_LIFECYCLE:
                callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_lifecycle_callback);
                break;
            case VIR_DOMAIN_EVENT_ID_REBOOT:
                callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_generic_callback);
                break;
            case VIR_DOMAIN_EVENT_ID_RTC_CHANGE:
                callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_rtcchange_callback);
                break;
            case VIR_DOMAIN_EVENT_ID_WATCHDOG:
                callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_watchdog_callback);
                break;
            case VIR_DOMAIN_EVENT_ID_IO_ERROR:
                callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_io_error_callback);
                break;
            case VIR_DOMAIN_EVENT_ID_IO_ERROR_REASON:
                callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_io_error_reason_callback);
                break;
            case VIR_DOMAIN_EVENT_ID_GRAPHICS:
                callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_graphics_callback);
                break;
            default:
                callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_generic_callback);
                break;
        }

        Local<Value> jscallback = arg_obj->Get(domain_event_callback_symbol);

        Persistent<Object> opaque = Persistent<Object>::New(Object::New());
        opaque->Set(domain_event_hypervisor_symbol, args.This());
        opaque->Set(domain_event_callback_symbol, jscallback);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
        int ret = virConnectDomainEventRegisterAny( hypervisor->conn_,
                                                    domain != NULL ? domain->domain() : NULL,
                                                    evtype, callback,
                                                    *opaque, domain_event_free);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(Integer::New(ret));
    }

    Handle<Value> Hypervisor::UnregisterDomainEvent(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        int callback_id = 0;

        if(args.Length() == 0 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a integer as argument to call this function")));
        }

        callback_id = args[0]->Int32Value();

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

        ret = virConnectDomainEventDeregisterAny(hypervisor->conn_, callback_id);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    int Hypervisor::domain_event_lifecycle_callback(virConnectPtr conn, //unused
                                                    virDomainPtr dom,
                                                    int event,
                                                    int detail,
                                                    void *opaque) {
        HandleScope scope;
        //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
        Domain *domain = new Domain();
        domain->domain_ = dom;
        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        /*The virDomainPtr object handle passed into the callback upon delivery
        of an event is only valid for the duration of execution of the callback
        If the callback wishes to keep the domain object after the callback,
        it shall take a reference to it, by calling virDomainRef*/
        virDomainRef(dom);

        Local<Value> argv[3];

        Persistent<Object> obj = static_cast<Object*>(opaque);
        Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

        Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
        Local<Function> callback = Local<Function>::Cast(callback_);

        Local<Object> data = Object::New();
        data->Set(domain_event_type_symbol, Integer::New(event));
        data->Set(domain_event_detail_symbol, Integer::New(detail));

        argv[0] = hyp;
        argv[1] = domain_obj; //FIXME change with domain->handle_
        argv[2] = data;

        TryCatch try_catch;

        callback->Call(hyp, 3, argv);

        if(try_catch.HasCaught()) {
            FatalException(try_catch);
        }

        return 0;
    }

    int Hypervisor::domain_event_generic_callback(  virConnectPtr conn, //unused
                                                    virDomainPtr dom,
                                                    void *opaque) {
        HandleScope scope;
        //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
        Domain *domain = new Domain();
        domain->domain_ = dom;
        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        /*The virDomainPtr object handle passed into the callback upon delivery
        of an event is only valid for the duration of execution of the callback
        If the callback wishes to keep the domain object after the callback,
        it shall take a reference to it, by calling virDomainRef*/
        virDomainRef(dom);

        Local<Value> argv[2];

        Persistent<Object> obj = static_cast<Object*>(opaque);
        Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

        Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
        Local<Function> callback = Local<Function>::Cast(callback_);

        argv[0] = hyp;
        argv[1] = domain_obj; //FIXME change with domain->handle_

        TryCatch try_catch;

        callback->Call(hyp, 2, argv);

        if(try_catch.HasCaught()) {
            FatalException(try_catch);
        }

        return 0;
    }

    int Hypervisor::domain_event_rtcchange_callback(virConnectPtr conn, //unused
                                                    virDomainPtr dom,
                                                    long long utcoffset,
                                                    void *opaque) {
        HandleScope scope;
        //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
        Domain *domain = new Domain();
        domain->domain_ = dom;
        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        /*The virDomainPtr object handle passed into the callback upon delivery
        of an event is only valid for the duration of execution of the callback
        If the callback wishes to keep the domain object after the callback,
        it shall take a reference to it, by calling virDomainRef*/
        virDomainRef(dom);

        Local<Value> argv[3];

        Persistent<Object> obj = static_cast<Object*>(opaque);
        Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

        Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
        Local<Function> callback = Local<Function>::Cast(callback_);

        Local<Object> data = Object::New();
        data->Set(domain_event_rtc_utcoffset_symbol, Number::New(utcoffset));

        argv[0] = hyp;
        argv[1] = domain_obj; //FIXME change with domain->handle_
        argv[2] = data;

        TryCatch try_catch;

        callback->Call(hyp, 3, argv);

        if(try_catch.HasCaught()) {
            FatalException(try_catch);
        }

        return 0;
    }

    int Hypervisor::domain_event_watchdog_callback( virConnectPtr conn, //unused
                                                    virDomainPtr dom,
                                                    int action,
                                                    void *opaque) {
        HandleScope scope;
        //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
        Domain *domain = new Domain();
        domain->domain_ = dom;
        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        /*The virDomainPtr object handle passed into the callback upon delivery
        of an event is only valid for the duration of execution of the callback
        If the callback wishes to keep the domain object after the callback,
        it shall take a reference to it, by calling virDomainRef*/
        virDomainRef(dom);

        Local<Value> argv[3];

        Persistent<Object> obj = static_cast<Object*>(opaque);
        Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

        Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
        Local<Function> callback = Local<Function>::Cast(callback_);

        Local<Object> data = Object::New();
        data->Set(domain_event_action_symbol, Integer::New(action));

        argv[0] = hyp;
        argv[1] = domain_obj; //FIXME change with domain->handle_
        argv[2] = data;

        TryCatch try_catch;

        callback->Call(hyp, 3, argv);

        if(try_catch.HasCaught()) {
            FatalException(try_catch);
        }

        return 0;
    }

    int Hypervisor::domain_event_io_error_callback( virConnectPtr conn, //unused
                                                    virDomainPtr dom,
                                                    const char *src_path,
                                                    const char *dev_alias,
                                                    int action,
                                                    void *opaque) {
        HandleScope scope;
        //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
        Domain *domain = new Domain();
        domain->domain_ = dom;
        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        /*The virDomainPtr object handle passed into the callback upon delivery
        of an event is only valid for the duration of execution of the callback
        If the callback wishes to keep the domain object after the callback,
        it shall take a reference to it, by calling virDomainRef*/
        virDomainRef(dom);

        Local<Value> argv[3];

        Persistent<Object> obj = static_cast<Object*>(opaque);
        Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

        Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
        Local<Function> callback = Local<Function>::Cast(callback_);

        Local<Object> data = Object::New();
        data->Set(domain_event_ioerror_srcpath_symbol, String::New(src_path));
        data->Set(domain_event_ioerror_devalias_symbol, String::New(dev_alias));
        data->Set(domain_event_action_symbol, Integer::New(action));

        argv[0] = hyp;
        argv[1] = domain_obj; //FIXME change with domain->handle_
        argv[2] = data;

        TryCatch try_catch;

        callback->Call(hyp, 3, argv);

        if(try_catch.HasCaught()) {
            FatalException(try_catch);
        }

        return 0;
    }

    int Hypervisor::domain_event_io_error_reason_callback(  virConnectPtr conn, //unused
                                                            virDomainPtr dom,
                                                            const char *src_path,
                                                            const char *dev_alias,
                                                            int action,
                                                            const char *reason,
                                                            void *opaque) {
        HandleScope scope;
        //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
        Domain *domain = new Domain();
        domain->domain_ = dom;
        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        /*The virDomainPtr object handle passed into the callback upon delivery
        of an event is only valid for the duration of execution of the callback
        If the callback wishes to keep the domain object after the callback,
        it shall take a reference to it, by calling virDomainRef*/
        virDomainRef(dom);

        Local<Value> argv[3];

        Persistent<Object> obj = static_cast<Object*>(opaque);
        Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

        Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
        Local<Function> callback = Local<Function>::Cast(callback_);

        Local<Object> data = Object::New();
        data->Set(domain_event_ioerror_srcpath_symbol, String::New(src_path));
        data->Set(domain_event_ioerror_devalias_symbol, String::New(dev_alias));
        data->Set(domain_event_ioerror_reason_symbol, String::New(reason));
        data->Set(domain_event_action_symbol, Integer::New(action));

        argv[0] = hyp;
        argv[1] = domain_obj; //FIXME change with domain->handle_
        argv[2] = data;

        TryCatch try_catch;

        callback->Call(hyp, 3, argv);

        if(try_catch.HasCaught()) {
            FatalException(try_catch);
        }

        return 0;
    }

    int Hypervisor::domain_event_graphics_callback( virConnectPtr conn, //unused
                                                    virDomainPtr dom,
                                                    int phase,
                                                    virDomainEventGraphicsAddressPtr local,
                                                    virDomainEventGraphicsAddressPtr remote,
                                                    const char *auth_scheme,
                                                    virDomainEventGraphicsSubjectPtr subject,
                                                    void *opaque) {
        HandleScope scope;
        //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
        Domain *domain = new Domain();
        domain->domain_ = dom;
        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        /*The virDomainPtr object handle passed into the callback upon delivery
        of an event is only valid for the duration of execution of the callback
        If the callback wishes to keep the domain object after the callback,
        it shall take a reference to it, by calling virDomainRef*/
        virDomainRef(dom);

        Local<Value> argv[3];

        Persistent<Object> obj = static_cast<Object*>(opaque);
        Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

        Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
        Local<Function> callback = Local<Function>::Cast(callback_);

        /*
        {local: {family: ipv4, node: 127.0.0.1, service: 80},
         remote: {family: ipv6, node: ::1, service: 80},
         subject: [{type: password, name: foo}, {type: password, name: fooo}],
         phase: 0 //VIR_DOMAIN_EVENT_GRAPHICS_CONNECT
         auth_scheme: foooscheme
         }
        */
        Local<String> lfamily;
        switch(local->family) {
            case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV4:
                lfamily = String::New("ipv4");
                break;
            case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV6:
                lfamily = String::New("ipv6");
                break;

        };

        Local<String> rfamily;
        switch(remote->family) {
            case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV4:
                rfamily = String::New("ipv4");
                break;
            case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV6:
                rfamily = String::New("ipv6");
                break;

        };

        Local<Object> local_ = Object::New();
        local_->Set(domain_event_graphics_family_sym, lfamily);
        local_->Set(domain_event_graphics_node_sym, String::New(local->node));
        local_->Set(domain_event_graphics_service_sym, String::New(local->service));

        Local<Object> remote_ = Object::New();
        remote_->Set(domain_event_graphics_family_sym, rfamily);
        remote_->Set(domain_event_graphics_node_sym, String::New(remote->node));
        remote_->Set(domain_event_graphics_service_sym, String::New(remote->service));

        int nidentity = subject->nidentity;
        Local<Array> subject_ = Array::New(nidentity);

        for(int i = 0; i < nidentity; i++) {
            Local<Object> identity = Object::New();
            identity->Set(domain_event_graphics_subjtype_sym, String::New(subject->identities[i].type));
            identity->Set(domain_event_graphics_subjname_sym, String::New(subject->identities[i].name));

            subject_->Set(Integer::New(i), identity);
        }

        Local<Object> data = Object::New();
        data->Set(domain_event_graphics_local_sym, local_);
        data->Set(domain_event_graphics_remote_sym, remote_);
        data->Set(domain_event_graphics_subject_sym, subject_);
        data->Set(domain_event_graphics_phase_sym, Integer::New(phase));
        data->Set(domain_event_graphics_authscheme_sym, String::New(auth_scheme));

        argv[0] = hyp;
        argv[1] = domain_obj; //FIXME change with domain->handle_
        argv[2] = data;

        TryCatch try_catch;

        callback->Call(hyp, 3, argv);

        if(try_catch.HasCaught()) {
            FatalException(try_catch);
        }
        return 0;
    }

    void Hypervisor::domain_event_free(void *opaque) {

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
