// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include "domain.h"

namespace NodeLibvirt {
    Persistent<FunctionTemplate> Domain::constructor_template;

    static Persistent<String> state_symbol;
    static Persistent<String> max_memory_symbol;
    static Persistent<String> memory_symbol;
    static Persistent<String> vcpus_number_symbol;
    static Persistent<String> cpu_time_symbol;
    static Persistent<String> number_symbol;
    static Persistent<String> cpu_symbol;
    static Persistent<String> affinity_symbol;
    static Persistent<String> usable_symbol;
    static Persistent<String> migration_uri_symbol;
    static Persistent<String> migration_name_symbol;
    static Persistent<String> migration_bandwidth_symbol;
    static Persistent<String> migration_flags_symbol;
    static Persistent<String> migration_hypervisor_symbol;

    void Domain::Initialize() {
        Local<FunctionTemplate> t = FunctionTemplate::New();

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "getId",
                                      Domain::GetId);
        NODE_SET_PROTOTYPE_METHOD(t, "getInfo",
                                      Domain::GetInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "toXml",
                                      Domain::ToXml);
        /*NODE_SET_PROTOTYPE_METHOD(t, "getJobInfo",
                                      Domain::GetJobInfo);*/
        NODE_SET_PROTOTYPE_METHOD(t, "getMaxMemory",
                                      Domain::GetMaxMemory);
        NODE_SET_PROTOTYPE_METHOD(t, "setMaxMemory",
                                      Domain::SetMaxMemory);
        NODE_SET_PROTOTYPE_METHOD(t, "setMemory",
                                      Domain::SetMemory);
        NODE_SET_PROTOTYPE_METHOD(t, "getMaxVcpus",
                                      Domain::GetMaxVcpus);
        NODE_SET_PROTOTYPE_METHOD(t, "getAutostart",
                                      Domain::GetAutostart);
        NODE_SET_PROTOTYPE_METHOD(t, "setAutostart",
                                      Domain::SetAutostart);
        NODE_SET_PROTOTYPE_METHOD(t, "getName",
                                      Domain::GetName);
        NODE_SET_PROTOTYPE_METHOD(t, "getOsType",
                                      Domain::GetOsType);
        /*NODE_SET_PROTOTYPE_METHOD(t, "getSchedParams",
                                      Domain::GetSchedParams);
        NODE_SET_PROTOTYPE_METHOD(t, "setSchedParams",
                                      Domain::SetSchedParams);
        NODE_SET_PROTOTYPE_METHOD(t, "getSchedType",
                                      Domain::GetSchedType);
        NODE_SET_PROTOTYPE_METHOD(t, "getSecurityLabel",
                                      Domain::GetSecurityLabel);*/
        NODE_SET_PROTOTYPE_METHOD(t, "getUUID",
                                      Domain::GetUUID);
        NODE_SET_PROTOTYPE_METHOD(t, "getVcpus",
                                      Domain::GetVcpus);
        NODE_SET_PROTOTYPE_METHOD(t, "setVcpus",
                                      Domain::SetVcpus);
        /*NODE_SET_PROTOTYPE_METHOD(t, "hasCurrentSnapshot",
                                      Domain::HasCurrentSnapshot);
        NODE_SET_PROTOTYPE_METHOD(t, "hasManagedSaveImage",
                                      Domain::HasManagedSaveImage);
        NODE_SET_PROTOTYPE_METHOD(t, "getInterfaceStats",
                                      Domain::GetInterfaceStats);*/
        NODE_SET_PROTOTYPE_METHOD(t, "isActive",
                                      Domain::IsActive);
        NODE_SET_PROTOTYPE_METHOD(t, "isPersistent",
                                      Domain::IsPersistent);
        /*NODE_SET_PROTOTYPE_METHOD(t, "managedSave",
                                      Domain::ManagedSave);
        NODE_SET_PROTOTYPE_METHOD(t, "managedSaveRemove",
                                      Domain::ManagedSaveRemove);
        NODE_SET_PROTOTYPE_METHOD(t, "memoryPeek",
                                      Domain::MemoryPeek);
        NODE_SET_PROTOTYPE_METHOD(t, "memoryStats",
                                      Domain::GetMemoryStats);*/
        NODE_SET_PROTOTYPE_METHOD(t, "migrate",
                                      Domain::Migrate);
        NODE_SET_PROTOTYPE_METHOD(t, "setMigrationMaxDowntime",
                                      Domain::SetMigrationMaxDowntime);
        NODE_SET_PROTOTYPE_METHOD(t, "pinVcpu",
                                      Domain::PinVcpu);
        NODE_SET_PROTOTYPE_METHOD(t, "reboot",
                                      Domain::Reboot);
        NODE_SET_PROTOTYPE_METHOD(t, "resume",
                                      Domain::Resume);
        NODE_SET_PROTOTYPE_METHOD(t, "save",
                                      Domain::Save);
        NODE_SET_PROTOTYPE_METHOD(t, "shutdown",
                                      Domain::Shutdown);
        NODE_SET_PROTOTYPE_METHOD(t, "start",
                                      Domain::Start);
        NODE_SET_PROTOTYPE_METHOD(t, "suspend",
                                      Domain::Suspend);

        /*NODE_SET_PROTOTYPE_METHOD(t, "revertToSnapshot",
                                      Domain::RevertToSnapshot);*/
        NODE_SET_PROTOTYPE_METHOD(t, "attachDevice",
                                      Domain::AttachDevice);
        NODE_SET_PROTOTYPE_METHOD(t, "detachDevice",
                                      Domain::DetachDevice);
        NODE_SET_PROTOTYPE_METHOD(t, "updateDevice",
                                      Domain::UpdateDevice);
        NODE_SET_PROTOTYPE_METHOD(t, "destroy",
                                      Domain::Destroy);
        NODE_SET_PROTOTYPE_METHOD(t, "undefine",
                                      Domain::Undefine);


        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->SetClassName(String::NewSymbol("Domain"));

        Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

        //Constants initialization
        //virDomainState
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_NOSTATE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_RUNNING);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCKED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_PAUSED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SHUTDOWN);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SHUTOFF);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_CRASHED);

        //virDomainDeviceModifyFlags
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_DEVICE_MODIFY_CURRENT);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_DEVICE_MODIFY_LIVE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_DEVICE_MODIFY_CONFIG);

        //virDomainMigrateFlags
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_LIVE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_PEER2PEER);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_TUNNELLED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_PERSIST_DEST);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_UNDEFINE_SOURCE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_PAUSED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_NON_SHARED_DISK);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_NON_SHARED_INC);

        //virDomainXMLFlags
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_XML_SECURE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_XML_INACTIVE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_XML_UPDATE_CPU);

        state_symbol        = NODE_PSYMBOL("state");
        max_memory_symbol   = NODE_PSYMBOL("max_memory");
        memory_symbol       = NODE_PSYMBOL("memory");
        vcpus_number_symbol = NODE_PSYMBOL("vcpus_number");
        cpu_time_symbol     = NODE_PSYMBOL("cpu_time");
        number_symbol       = NODE_PSYMBOL("number");
        cpu_symbol          = NODE_PSYMBOL("cpu");
        affinity_symbol     = NODE_PSYMBOL("affinity");
        usable_symbol       = NODE_PSYMBOL("usable");
        migration_uri_symbol            = NODE_PSYMBOL("dest_uri");
        migration_name_symbol           = NODE_PSYMBOL("dest_name");
        migration_bandwidth_symbol      = NODE_PSYMBOL("bandwidth");
        migration_flags_symbol          = NODE_PSYMBOL("flags");
        migration_hypervisor_symbol     = NODE_PSYMBOL("dest_hypervisor");
    }

    Domain::~Domain() {
        if(domain_ != NULL) {
            virDomainFree(domain_);
        }
    }

    /*Handle<Value> Domain::New(const Arguments& args) {
        HandleScope scope;

        Domain *domain = new Domain();
        domain->Wrap(args.This());

        return args.This();
    }*/

    Handle<Value> Domain::Create(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        const char* xml = NULL;

        int argsl = args.Length();

        if(argsl == 0) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at least one argument")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }
        String::Utf8Value xml_(args[0]->ToString());
        xml = ToCString(xml_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Domain *domain = new Domain();
        domain->domain_ = virDomainCreateXML(hypervisor->connection(), xml, flags);

        if(domain->domain_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        return scope.Close(domain_obj);
    }

    Handle<Value> Domain::Define(const Arguments& args) {
        HandleScope scope;

        const char* xml = NULL;
        int argsl = args.Length();

        if(argsl == 0) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at least one argument")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value xml_(args[0]->ToString());
        xml = ToCString(xml_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Domain *domain = new Domain();
        domain->domain_ = virDomainDefineXML(hypervisor->connection(), xml);

        if(domain->domain_) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();

        domain->Wrap(domain_obj);

        return scope.Close(domain_obj);
    }

    Handle<Value> Domain::Undefine(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        ret = virDomainUndefine(domain->domain_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::LookupById(const Arguments& args) {
        HandleScope scope;

        virConnectPtr conn = NULL;
        int id = -1;

        if(args.Length() == 0 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid Domain Id.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        id = args[0]->Int32Value();

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Domain *domain = new Domain();
        domain->domain_ = virDomainLookupByID(hypervisor->connection(), id);
        if(domain->domain_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();

        domain->Wrap(domain_obj);

        return scope.Close(domain_obj);
    }

    Handle<Value> Domain::LookupByName(const Arguments& args) {
        HandleScope scope;

        const char* name = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid Domain name.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value name_(args[0]->ToString());

        name = ToCString(name_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Domain *domain = new Domain();
        domain->domain_ = virDomainLookupByName(hypervisor->connection(), name);

        if(domain->domain_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();

        domain->Wrap(domain_obj);

        return scope.Close(domain_obj);
    }

    Handle<Value> Domain::LookupByUUID(const Arguments& args) {
        HandleScope scope;
        const char* uuid = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a UUID string.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value uuid_(args[0]->ToString());

        uuid = ToCString(uuid_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Domain *domain = new Domain();
        domain->domain_ = virDomainLookupByUUIDString(hypervisor->connection(), uuid);

        if(domain->domain_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();

        domain->Wrap(domain_obj);

        return scope.Close(domain_obj);
    }

    Handle<Value> Domain::GetId(const Arguments& args) {
        HandleScope scope;
        unsigned int id = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        id = virDomainGetID(domain->domain_);

        if(id == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return Integer::NewFromUnsigned(id);
    }

    Handle<Value> Domain::GetInfo(const Arguments& args) {
        HandleScope scope;
        virDomainInfo info;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainGetInfo(domain->domain_, &info);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        Local<Object> object = Object::New();

        object->Set(state_symbol, Integer::New(info.state)); //virDomainState
        object->Set(max_memory_symbol, Number::New(info.maxMem)); //KBytes
        object->Set(memory_symbol, Number::New(info.memory)); //KBytes
        object->Set(vcpus_number_symbol, Integer::New(info.nrVirtCpu));
        object->Set(cpu_time_symbol, Number::New(info.cpuTime)); //nanoseconds

        return object;
    }

    Handle<Value> Domain::GetName(const Arguments& args) {
        HandleScope scope;
        const char *name = NULL;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        name = virDomainGetName(domain->domain_);

        if(name == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return String::New(name);
    }

    Handle<Value> Domain::GetUUID(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        char *uuid = new char[VIR_UUID_STRING_BUFLEN];

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainGetUUIDString(domain->domain_, uuid);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            delete[] uuid;
            return Null();
        }

        Local<String> uuid_str = String::New(uuid);

        delete[] uuid;

        return uuid_str;
    }

    Handle<Value> Domain::GetAutostart(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        int autostart_;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainGetAutostart(domain->domain_, &autostart_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        bool autostart = autostart_ == 0 ? true : false;

        return Boolean::New(autostart);
    }

    Handle<Value> Domain::SetAutostart(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsBoolean()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a boolean argument")));
        }

        bool autostart = args[0]->IsTrue();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainSetAutostart(domain->domain_, autostart ? 0 : 1);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::GetMaxMemory(const Arguments& args) {
        HandleScope scope;
        unsigned long memory = 0;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        memory = virDomainGetMaxMemory(domain->domain_);

        if(memory == 0) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return Number::New(memory);
    }

    Handle<Value> Domain::SetMaxMemory(const Arguments& args) {
        HandleScope scope;
        unsigned long memory = 0;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid amount of memory")));
        }

        memory = args[0]->Int32Value();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainSetMaxMemory(domain->domain_, memory);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::SetMemory(const Arguments& args) {
        HandleScope scope;
        unsigned long memory = 0;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid amount of memory")));
        }

        memory = args[0]->Int32Value();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainSetMemory(domain->domain_, memory);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::GetOsType(const Arguments& args) {
        HandleScope scope;
        char *os_type = NULL;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        os_type = virDomainGetOSType(domain->domain_);

        if(os_type == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return String::New(os_type);
    }

    Handle<Value> Domain::GetMaxVcpus(const Arguments& args) {
        HandleScope scope;
        int vcpus = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        vcpus = virDomainGetMaxVcpus(domain->domain_);

        if(vcpus == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return Integer::New(vcpus);
    }

    Handle<Value> Domain::IsActive(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainIsActive(domain->domain_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        bool is_active = ret == 1 ? true : false;

        return Boolean::New(is_active);
    }

    Handle<Value> Domain::IsPersistent(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainIsPersistent(domain->domain_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        bool is_persistent = ret == 1 ? true : false;

        return Boolean::New(is_persistent);
    }

    Handle<Value> Domain::Reboot(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainReboot(domain->domain_, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::Save(const Arguments& args) {
        HandleScope scope;
        const char* path = NULL;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as function argument")));
        }

        String::Utf8Value path_(args[0]->ToString());
        path = ToCString(path_);

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainSave(domain->domain_, path);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::Restore(const Arguments& args) {
        HandleScope scope;
        const char *path = NULL;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as function argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }

        String::Utf8Value path_(args[0]->ToString());
        path = ToCString(path_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        ret = virDomainRestore(hypervisor->connection(), path);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return scope.Close(False());
        }

        return scope.Close(True());
    }

    Handle<Value> Domain::Suspend(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainSuspend(domain->domain_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::Resume(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainResume(domain->domain_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }
        return True();
    }

    Handle<Value> Domain::Shutdown(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        ret = virDomainShutdown(domain->domain_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::Start(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainCreate(domain->domain_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }
        return True();
    }

    Handle<Value> Domain::GetVcpus(const Arguments& args) {
        HandleScope scope;

        virDomainInfo info;
        virNodeInfo nodeinfo;
        virVcpuInfoPtr cpuinfo = NULL;
        unsigned char *cpumap = NULL;
        int ncpus;
        int cpumaplen;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainGetInfo(domain->domain_, &info);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        ret = virNodeGetInfo(virDomainGetConnect(domain->domain_), &nodeinfo);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        cpuinfo = (virVcpuInfoPtr) malloc(sizeof(*cpuinfo) * info.nrVirtCpu);
        if(cpuinfo == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return Null();
        }

        cpumaplen = VIR_CPU_MAPLEN(VIR_NODEINFO_MAXCPUS(nodeinfo));

        cpumap = (unsigned char*)malloc(info.nrVirtCpu * cpumaplen);
        if(cpumap == NULL) {
            free(cpuinfo);
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return Null();
        }

        ncpus = virDomainGetVcpus(domain->domain_, cpuinfo, info.nrVirtCpu, cpumap, cpumaplen);

        if(ncpus < 0) {
            free(cpuinfo);
            free(cpumap);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Array> vcpus = Array::New(info.nrVirtCpu);
        for(int i = 0; i < info.nrVirtCpu; i++) {
            Local<Object> obj = Object::New();
            obj->Set(number_symbol, Integer::New(cpuinfo[i].number));
            obj->Set(state_symbol, Integer::New(cpuinfo[i].state));
            obj->Set(cpu_time_symbol, Number::New(cpuinfo[i].cpuTime)); //nanoseconds
            obj->Set(cpu_symbol, Integer::New(cpuinfo[i].cpu));

            int maxcpus =  VIR_NODEINFO_MAXCPUS(nodeinfo);
            Local<Array> affinity = Array::New(maxcpus);
            for(int j = 0; j < maxcpus; j++) {
                Local<Object> cpu = Object::New();
                cpu->Set(cpu_symbol, Integer::New(j));
                cpu->Set(usable_symbol, Boolean::New(VIR_CPU_USABLE(cpumap, cpumaplen, i, j)));

                affinity->Set(Integer::New(j), cpu);
            }
            obj->Set(affinity_symbol, affinity);

            vcpus->Set(Integer::New(i), obj);
        }
        free(cpuinfo);
        free(cpumap);

        return vcpus;
    }

    Handle<Value> Domain::SetVcpus(const Arguments& args) {
        HandleScope scope;
        unsigned int vcpus = 0;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a number")));
        }

        vcpus = args[0]->Int32Value();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainSetVcpus(domain->domain_, vcpus);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::Migrate(const Arguments& args) {
        HandleScope scope;
        const char* dest_uri = NULL;
        const char* dest_name = NULL;
        unsigned long flags = 0;
        unsigned long bandwidth = 0;
        int ret = -1;

        if(args.Length() == 0) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify arguments to invoke this function")));
        }

        if(!args[0]->IsObject()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an object as first argument")));
        }

        Local<Object> args_ = args[0]->ToObject();

        if(!args_->Has(migration_uri_symbol)) {
            return ThrowException(Exception::TypeError(
            String::New("You must have set property dest_uri in the object")));
        }

        //dest_uri
        String::Utf8Value dest_uri_(args_->Get(migration_uri_symbol));
        dest_uri = ToCString(dest_uri_);

        //dest_name
        if(args_->Has(migration_name_symbol)) {
            String::Utf8Value dest_name_(args_->Get(migration_name_symbol));
            dest_name = ToCString(dest_name_);
        }

        //flags
        if(args_->Has(migration_flags_symbol)){
            Local<Array> flags_ = Local<Array>::Cast(args_->Get(migration_flags_symbol));
            unsigned int length = flags_->Length();

            for (int i = 0; i < length; i++) {
                flags |= flags_->Get(Integer::New(i))->Int32Value();
            }
        }

        //bandwidth (Mbps)
        if(args_->Has(migration_bandwidth_symbol)) {
            bandwidth = args_->Get(migration_bandwidth_symbol)->Int32Value();
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        if(args_->Has(migration_hypervisor_symbol)) {
            Local<Object> hyp_obj = args_->Get(migration_hypervisor_symbol)->ToObject();

            if(!Hypervisor::HasInstance(hyp_obj)) {
                return ThrowException(Exception::TypeError(
                String::New("You must specify a Hypervisor object instance")));
            }

            Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

            Domain *migrated_domain = new Domain();
            migrated_domain->domain_ = virDomainMigrate(domain->domain_,
                                                        hypervisor->connection(),
                                                        flags,
                                                        dest_name,
                                                        dest_uri,
                                                        bandwidth);

            if(migrated_domain->domain_ == NULL) {
                ThrowException(Error::New(virGetLastError()));
                return False();
            }

            migrated_domain->Wrap(args.This());

            return migrated_domain->constructor_template->GetFunction()->NewInstance();
        } else {
            ret = virDomainMigrateToURI(domain->domain_, dest_uri, flags, dest_name, bandwidth);
        }

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::SetMigrationMaxDowntime(const Arguments& args) {
        HandleScope scope;
        long long downtime = 0;
        unsigned int flags = 0;
        int ret = -1;

        if(args.Length() == 0) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify arguments to invoke this function")));
        }

        if(!args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a number as first argument")));
        }

        downtime = args[0]->Int32Value();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainMigrateSetMaxDowntime(domain->domain_, downtime, flags);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::PinVcpu(const Arguments& args) {
        HandleScope scope;
        virNodeInfo nodeinfo;
        unsigned char *cpumap = NULL;
        int cpumaplen;
        int vcpu;
        int ret = -1;

        if(args.Length() < 2) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify two arguments")));
        }

        if(!args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("The first argument must be an integer")));
        }

        if(!args[1]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("The second argument must be an array of objects")));
        }

        vcpu = args[0]->Int32Value();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virNodeGetInfo(virDomainGetConnect(domain->domain_), &nodeinfo);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        int maxcpus = VIR_NODEINFO_MAXCPUS(nodeinfo);

        cpumaplen = VIR_CPU_MAPLEN(maxcpus);
        cpumap = (unsigned char*)malloc(cpumaplen);
        if(cpumap == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return False();
        }

        Local<Array> cpus = Local<Array>::Cast(args[1]);
        int ncpus = cpus->Length();

        for(int i = 0; i < ncpus; i++) {
            if(i > maxcpus) {
                break;
            }

            if(!cpus->Get(Integer::New(i))->IsObject()) {
                return ThrowException(Exception::TypeError(
                String::New("The second argument must be an array of objects")));
            }

            Local<Object> cpu = cpus->Get(Integer::New(i))->ToObject();
            bool usable = cpu->Get(usable_symbol)->IsTrue();

            if(usable) {
                VIR_USE_CPU(cpumap, cpu->Get(cpu_symbol)->Int32Value());
            } else {
                VIR_UNUSE_CPU(cpumap, cpu->Get(cpu_symbol)->Int32Value());
            }
        }

        ret = virDomainPinVcpu(domain->domain_, vcpu, cpumap, cpumaplen);
        free(cpumap);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::AttachDevice(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        const char* xml = NULL;
        int ret = -1;

        int argsl = args.Length();

        if(argsl == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string")));
        }

        if(argsl == 2 && !args[1]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an object with flags")));
        }

        String::Utf8Value xml_(args[0]->ToString());

        xml = ToCString(xml_);

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[1]);
        unsigned int length = flags_->Length();

        for (int i = 0; i < length; i++) {
            flags |= flags_->Get(Integer::New(i))->Int32Value();
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        if(flags > 0) {
            ret = virDomainAttachDeviceFlags(domain->domain_, xml, flags);
        } else {
            ret = virDomainAttachDevice(domain->domain_, xml);
        }

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::DetachDevice(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        const char* xml = NULL;
        int ret = -1;

        int argsl = args.Length();

        if(argsl == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string")));
        }

        if(argsl == 2 && !args[1]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an object with flags")));
        }

        String::Utf8Value xml_(args[0]->ToString());

        xml = ToCString(xml_);

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[1]);
        unsigned int length = flags_->Length();

        for (int i = 0; i < length; i++) {
            flags |= flags_->Get(Integer::New(i))->Int32Value();
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        if(flags > 0) {
            ret = virDomainDetachDeviceFlags(domain->domain_, xml, flags);
        } else {
            ret = virDomainDetachDevice(domain->domain_, xml);
        }

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }
        return True();
    }

    Handle<Value> Domain::UpdateDevice(const Arguments& args) {
        HandleScope scope;
        const char* xml = NULL;
        unsigned int flags = 0;
        int ret = -1;

        if(args.Length() < 2) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify two arguments to invoke this function")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        if(!args[1]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an array as second argument")));
        }

        String::Utf8Value xml_(args[0]->ToString());

        xml = ToCString(xml_);

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[1]);
        unsigned int length = flags_->Length();

        for (int i = 0; i < length; i++) {
            flags |= flags_->Get(Integer::New(i))->Int32Value();
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainUpdateDeviceFlags(domain->domain_, xml, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::Destroy(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainDestroy(domain->domain_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        if(domain->domain_ != NULL) {
            virDomainFree(domain->domain_);
        }

        return True();
    }

    Handle<Value> Domain::ToXml(const Arguments& args) {
        HandleScope scope;
        char* xml_ = NULL;
        int flags = 0;

        if(args.Length() == 0 || !args[0]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an array as argument to invoke this function")));
        }

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[0]);
        unsigned int length = flags_->Length();

        for (int i = 0; i < length; i++) {
            flags |= flags_->Get(Integer::New(i))->Int32Value();
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        xml_ = virDomainGetXMLDesc(domain->domain_, flags);

        if(xml_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> xml = String::New(xml_);

        free(xml_);

        return xml;
    }

} //namespace NodeLibvirt

