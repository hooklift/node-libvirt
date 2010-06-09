// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include "domain.h"

//FIXME better error reporting
namespace NodeLibvirt {
    Persistent<FunctionTemplate> Domain::constructor_template;

    static Persistent<String> state_symbol;
    static Persistent<String> max_memory_symbol;
    static Persistent<String> memory_symbol;
    static Persistent<String> vcpus_number_symbol;
    static Persistent<String> cpu_time_symbol;

    void Domain::Initialize() {
        Local<FunctionTemplate> t = FunctionTemplate::New();

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "getId",
                                      Domain::GetId);
        NODE_SET_PROTOTYPE_METHOD(t, "getInfo",
                                      Domain::GetInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "getJobInfo",
                                      Domain::GetJobInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "getMaxMemory",
                                      Domain::GetMaxMemory);
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
        NODE_SET_PROTOTYPE_METHOD(t, "getSchedParams",
                                      Domain::GetSchedParams);
        NODE_SET_PROTOTYPE_METHOD(t, "setSchedParams",
                                      Domain::SetSchedParams);
        NODE_SET_PROTOTYPE_METHOD(t, "getSchedType",
                                      Domain::GetSchedType);
        NODE_SET_PROTOTYPE_METHOD(t, "getSecurityLabel",
                                      Domain::GetSecurityLabel);
        NODE_SET_PROTOTYPE_METHOD(t, "getUUID",
                                      Domain::GetUUID);
        NODE_SET_PROTOTYPE_METHOD(t, "getVcpus",
                                      Domain::GetVcpus);
        NODE_SET_PROTOTYPE_METHOD(t, "setVcpus",
                                      Domain::SetVcpus);
        NODE_SET_PROTOTYPE_METHOD(t, "hasCurrentSnapshot",
                                      Domain::HasCurrentSnapshot);
        NODE_SET_PROTOTYPE_METHOD(t, "hasManagedSaveImage",
                                      Domain::HasManagedSaveImage);
        NODE_SET_PROTOTYPE_METHOD(t, "getInterfaceStats",
                                      Domain::GetInterfaceStats);
        NODE_SET_PROTOTYPE_METHOD(t, "isActive",
                                      Domain::IsActive);
        NODE_SET_PROTOTYPE_METHOD(t, "isPersistent",
                                      Domain::IsPersistent);
        NODE_SET_PROTOTYPE_METHOD(t, "managedSave",
                                      Domain::ManagedSave);
        NODE_SET_PROTOTYPE_METHOD(t, "managedSaveRemove",
                                      Domain::ManagedSaveRemove);
        NODE_SET_PROTOTYPE_METHOD(t, "memoryPeek",
                                      Domain::MemoryPeek);
        NODE_SET_PROTOTYPE_METHOD(t, "memoryStats",
                                      Domain::GetMemoryStats);
        NODE_SET_PROTOTYPE_METHOD(t, "migrate",
                                      Domain::Migrate);
        NODE_SET_PROTOTYPE_METHOD(t, "migrateSetMaxDowntime",
                                      Domain::MigrateSetMaxDowntime);
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
        NODE_SET_PROTOTYPE_METHOD(t, "suspend",
                                      Domain::Suspend);
        NODE_SET_PROTOTYPE_METHOD(t, "undefine",
                                      Domain::Undefine);
        NODE_SET_PROTOTYPE_METHOD(t, "revertToSnapshot",
                                      Domain::RevertToSnapshot);
        NODE_SET_PROTOTYPE_METHOD(t, "destroy",
                                      Domain::Destroy);


        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->SetClassName(String::NewSymbol("Domain"));

        state_symbol        = NODE_PSYMBOL("state");
        max_memory_symbol   = NODE_PSYMBOL("max_memory");
        memory_symbol       = NODE_PSYMBOL("memory");
        vcpus_number_symbol = NODE_PSYMBOL("vcpus_number");
        cpu_time_symbol     = NODE_PSYMBOL("cpu_time");
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
        virConnectPtr conn = NULL;
        unsigned int flags = 0;
        bool persistent = false;

        int argsl = args.Length();

        if(argsl == 0) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at least one argument")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        if(argsl == 2 && args[1]->IsBoolean()) {
            persistent = args[1]->IsTrue();
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }
        String::Utf8Value xml(args[0]->ToString());


        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        conn = hypervisor->connection();

        Domain *domain = new Domain();
        return scope.Close(domain->create(ToCString(xml), persistent, conn, flags));
    }

    Handle<Value> Domain::create(const char* xml, bool persistent,
                                 virConnectPtr conn, unsigned int flags) {
        if(persistent) {
            //http://libvirt.org/html/libvirt-libvirt.html#virDomainDefineXML
            domain_ = virDomainDefineXML(conn, xml);
        } else {
            //http://libvirt.org/html/libvirt-libvirt.html#virDomainCreateXML
            domain_ = virDomainCreateXML(conn, xml, flags);
        }

        if(domain_ == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }

        return new_js_instance();
    }

    Handle<Value> Domain::LookupById(const Arguments& args) {
        HandleScope scope;

        virConnectPtr conn = NULL;

        if(args.Length() == 0 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid Domain Id.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }

        int id = args[0]->Int32Value();

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        conn = hypervisor->connection();

        Domain *domain = new Domain();
        return scope.Close(domain->lookup_by_id(conn, id));
    }

    Handle<Value> Domain::lookup_by_id(virConnectPtr conn, int id) {

        domain_ = virDomainLookupByID(conn, id);

        if(domain_ == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }

        return new_js_instance();
    }

    Handle<Value> Domain::LookupByName(const Arguments& args) {
        HandleScope scope;

        virConnectPtr conn = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid Domain name.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }

        String::Utf8Value name_(args[0]->ToString());

        const char* name = ToCString(name_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        conn = hypervisor->connection();

        Domain *domain = new Domain();
        return scope.Close(domain->lookup_by_name(conn, name));
    }

    Handle<Value> Domain::lookup_by_name(virConnectPtr conn, const char* name) {

        domain_ = virDomainLookupByName(conn, name);

        if(domain_ == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }

        return new_js_instance();
    }

    Handle<Value> Domain::LookupByUUID(const Arguments& args) {
        HandleScope scope;

        virConnectPtr conn = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a UUID string.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }

        String::Utf8Value uuid_(args[0]->ToString());

        const char* uuid = ToCString(uuid_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        conn = hypervisor->connection();

        Domain *domain = new Domain();
        return scope.Close(domain->lookup_by_uuid(conn, uuid));
    }

    Handle<Value> Domain::lookup_by_uuid(virConnectPtr conn, const char* uuid) {

        domain_ = virDomainLookupByUUIDString(conn, uuid);

        if(domain_ == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                LIBVIRT_THROW_EXCEPTION(error->message);
            }
            return Null();
        }

        return new_js_instance();
    }

    Handle<Value> Domain::new_js_instance() {

        Local<Object> object = constructor_template->GetFunction()->NewInstance();

        //Constants initialization
        //virDomainState
        NODE_DEFINE_CONSTANT(object, VIR_DOMAIN_NOSTATE);
        NODE_DEFINE_CONSTANT(object, VIR_DOMAIN_RUNNING);
        NODE_DEFINE_CONSTANT(object, VIR_DOMAIN_BLOCKED);
        NODE_DEFINE_CONSTANT(object, VIR_DOMAIN_PAUSED);
        NODE_DEFINE_CONSTANT(object, VIR_DOMAIN_SHUTDOWN);
        NODE_DEFINE_CONSTANT(object, VIR_DOMAIN_SHUTOFF);
        NODE_DEFINE_CONSTANT(object, VIR_DOMAIN_CRASHED);

        return object;
    }

    Handle<Value> Domain::GetId(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->get_id();
    }

    Handle<Value> Domain::get_id() {
        unsigned int id = virDomainGetID(domain_);

        if(id == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }

        return Integer::NewFromUnsigned(id);
    }

    Handle<Value> Domain::GetInfo(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->get_info();
    }

    Handle<Value> Domain::get_info() {
        virDomainInfo info;
        int ret = virDomainGetInfo(domain_, &info);

        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
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

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->get_name();
    }

    Handle<Value> Domain::get_name() {
        const char *name = virDomainGetName(domain_);

        if(name == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }

        return String::New(name);
    }

    Handle<Value> Domain::GetUUID(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->get_uuid();
    }

    Handle<Value> Domain::get_uuid() {
        char *uuid = new char[VIR_UUID_STRING_BUFLEN];

        int ret = virDomainGetUUIDString(domain_, uuid);

        if(ret == -1) {
            virError *error = virGetLastError();
            delete[] uuid;
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }

        Local<String> uuid_str = String::New(uuid);

        delete[] uuid;

        return uuid_str;
    }

    Handle<Value> Domain::GetAutostart(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->get_autostart();
    }

    Handle<Value> Domain::get_autostart() {
        int autostart_;

        int ret = virDomainGetAutostart(domain_, &autostart_);
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }

        bool autostart = autostart_ == 0 ? true : false;

        return Boolean::New(autostart);
    }

    Handle<Value> Domain::SetAutostart(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsBoolean()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a boolean argument")));
        }

        bool autostart = args[0]->IsTrue();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->set_autostart(autostart);
    }

    Handle<Value> Domain::set_autostart(bool autostart) {
        int ret = virDomainSetAutostart(domain_, autostart ? 0 : 1);
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }
        return Undefined();
    }

    Handle<Value> Domain::GetMaxMemory(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->get_max_memory();
    }

    Handle<Value> Domain::get_max_memory() {
        unsigned long memory = virDomainGetMaxMemory(domain_);

        if(memory == 0) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }

        return Number::New(memory);
    }

    Handle<Value> Domain::SetMaxMemory(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid amount of memory")));
        }

        unsigned long memory = args[0]->Int32Value();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->set_max_memory(memory);
    }

    Handle<Value> Domain::set_max_memory(unsigned long memory) {
        int ret = virDomainSetMaxMemory(domain_, memory);
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }
        return Undefined();
    }

      Handle<Value> Domain::SetMemory(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid amount of memory")));
        }

        unsigned long memory = args[0]->Int32Value();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->set_memory(memory);
    }

    Handle<Value> Domain::set_memory(unsigned long memory) {
        int ret = virDomainSetMemory(domain_, memory);
        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }
        return Undefined();
    }

    Handle<Value> Domain::GetOsType(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->get_os_type();
    }

    Handle<Value> Domain::get_os_type() {
        char *os_type = virDomainGetOSType(domain_);

        if(os_type == NULL) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }

        return String::New(os_type);
    }

    Handle<Value> Domain::GetMaxVcpus(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->get_max_vcpus();
    }

    Handle<Value> Domain::get_max_vcpus() {
        int vcpus = virDomainGetMaxVcpus(domain_);

        if(vcpus == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }

        return Integer::New(vcpus);
    }

    Handle<Value> Domain::IsActive(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->is_active();
    }

    Handle<Value> Domain::is_active() {
        int ret = virDomainIsActive(domain_);

        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }
        bool is_active = ret == 1 ? true : false;

        return Boolean::New(is_active);
    }

    Handle<Value> Domain::IsPersistent(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->is_persistent();
    }

    Handle<Value> Domain::is_persistent() {
        int ret = virDomainIsPersistent(domain_);

        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                return ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return Null();
        }
        bool is_persistent = ret == 1 ? true : false;

        return Boolean::New(is_persistent);
    }

    Handle<Value> Domain::Reboot(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->reboot(flags);
    }

    Handle<Value> Domain::reboot(unsigned int flags) {
        int ret = virDomainReboot(domain_, flags);

        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return False();
        }

        return True();
    }

    Handle<Value> Domain::Save(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as function argument")));
        }

        String::Utf8Value path_(args[0]->ToString());
        const char *path = ToCString(path_);

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        domain->save(path);
    }

    Handle<Value> Domain::save(const char* path) {
        int ret = virDomainSave(domain_, path);

        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return False();
        }
        return True();
    }

    Handle<Value> Domain::Restore(const Arguments& args) {
        HandleScope scope;
        virConnectPtr conn = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as function argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        conn = hypervisor->connection();

        String::Utf8Value path_(args[0]->ToString());
        const char *path = ToCString(path_);

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        scope.Close(domain->restore(conn, path));
    }

    Handle<Value> Domain::restore(virConnectPtr conn, const char* path) {
        int ret = virDomainRestore(conn, path);

        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return False();
        }
        return True();
    }

    Handle<Value> Domain::Suspend(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        domain->suspend();
    }

    Handle<Value> Domain::suspend() {
        int ret = virDomainSuspend(domain_);

        if(ret == -1) {
          virError *error = virGetLastError();
            if(error != NULL) {
                ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return False();
        }
        return True();
    }

    Handle<Value> Domain::Resume(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        domain->resume();
    }

    Handle<Value> Domain::resume() {
        int ret = virDomainResume(domain_);

        if(ret == -1) {
          virError *error = virGetLastError();
            if(error != NULL) {
                ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return False();
        }
        return True();
    }

    Handle<Value> Domain::Shutdown(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        domain->shutdown();
    }

    Handle<Value> Domain::shutdown() {
        int ret = virDomainShutdown(domain_);

        if(ret == -1) {
          virError *error = virGetLastError();
            if(error != NULL) {
                ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return False();
        }
        return True();
    }

    //virDomainMigrate

    Handle<Value> Domain::Destroy(const Arguments& args) {
        HandleScope scope;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        return domain->destroy();
    }

    Handle<Value> Domain::destroy() {
        if(domain_ == NULL) {
            return False();
        }

        int ret = virDomainDestroy(domain_);

        if(ret == -1) {
            virError *error = virGetLastError();
            if(error != NULL) {
                ThrowException(Exception::Error(
                String::New(error->message)));
            }
            return False();
        } else {
            if(domain_ != NULL) {
                virDomainFree(domain_);
            }
            return True();
        }
    }
} //namespace NodeLibvirt

