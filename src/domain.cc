// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include <string.h>
#include <node_buffer.h>
#include "domain.h"

#define NOARGS_WORKER_METHOD(name, worker)                                  \
                                                                            \
NAN_METHOD(Domain::name) {                                                          \
  NanScope();                                                               \
                                                                            \
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());                 \
                                                                            \
  if (args.Length() == 1 && !args[0]->IsFunction()) {                       \
    return ThrowException(Exception::TypeError(                             \
    String::New("You must specify a function as first argument")));         \
  }                                                                         \
                                                                            \
  NanCallback *callback = new NanCallback(args[0].As<Function>());          \
                                                                            \
  NanAsyncQueueWorker(new worker(callback, domain->domain_));               \
                                                                            \
  NanReturnUndefined();                                                     \
}

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

    //Jobinfo symbols
    static Persistent<String> type_symbol;
    static Persistent<String> time_symbol;
    static Persistent<String> data_symbol;
    static Persistent<String> file_symbol;
    static Persistent<String> elapsed_symbol;
    static Persistent<String> remaining_symbol;
    static Persistent<String> total_symbol;
    static Persistent<String> processed_symbol;

    //SecurityLabel symbols
    static Persistent<String> label_symbol;
    static Persistent<String> enforcing_symbol;

    //memory stat symbols
    static Persistent<String> memory_stat_swap_in_symbol;
    static Persistent<String> memory_stat_swap_out_symbol;
    static Persistent<String> memory_stat_major_fault_symbol;
    static Persistent<String> memory_stat_minor_fault_symbol;
    static Persistent<String> memory_stat_unused_symbol;
    static Persistent<String> memory_stat_available_symbol;

    //block stat symbols
    static Persistent<String> block_stat_rd_req_symbol;
    static Persistent<String> block_stat_rd_bytes_symbol;
    static Persistent<String> block_stat_wr_req_symbol;
    static Persistent<String> block_stat_wr_bytes_symbol;
    static Persistent<String> block_stat_errs_symbol;

    //block info symbols
    static Persistent<String> block_info_capacity_symbol;
    static Persistent<String> block_info_allocation_symbol;
    static Persistent<String> block_info_physical_symbol;

    //domain network interface statistics symbols
    static Persistent<String> nwiface_stat_rx_bytes_symbol;
    static Persistent<String> nwiface_stat_rx_packets_symbol;
    static Persistent<String> nwiface_stat_rx_errors_symbol;
    static Persistent<String> nwiface_stat_rx_drop_symbol;
    static Persistent<String> nwiface_stat_tx_bytes_symbol;
    static Persistent<String> nwiface_stat_tx_packets_symbol;
    static Persistent<String> nwiface_stat_tx_errors_symbol;
    static Persistent<String> nwiface_stat_tx_drop_symbol;

    void Domain::Initialize() {
        Local<FunctionTemplate> t = FunctionTemplate::New();

//        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "getId",
                                      Domain::GetId);
        NODE_SET_PROTOTYPE_METHOD(t, "toXml",
                                      Domain::ToXml);
        NODE_SET_PROTOTYPE_METHOD(t, "getJobInfo",
                                      Domain::GetJobInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "abortCurrentJob",
                                      Domain::AbortCurrentJob);
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
        NODE_SET_PROTOTYPE_METHOD(t, "getInfo",
                                      Domain::GetInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "getSchedParams",
                                      Domain::GetSchedParams);
        NODE_SET_PROTOTYPE_METHOD(t, "setSchedParams",
                                      Domain::SetSchedParams);
        /*NODE_SET_PROTOTYPE_METHOD(t, "getSchedType",
                                      Domain::GetSchedType); */ //It's necessary this function?
        NODE_SET_PROTOTYPE_METHOD(t, "getSecurityLabel",
                                      Domain::GetSecurityLabel);
        NODE_SET_PROTOTYPE_METHOD(t, "hasManagedImage",
                                      Domain::HasManagedImage);
        NODE_SET_PROTOTYPE_METHOD(t, "saveManagedImage",
                                      Domain::SaveManagedImage);
        NODE_SET_PROTOTYPE_METHOD(t, "removeManagedImage",
                                      Domain::RemoveManagedImage);
        NODE_SET_PROTOTYPE_METHOD(t, "memoryPeek",
                                      Domain::MemoryPeek);
        NODE_SET_PROTOTYPE_METHOD(t, "getMemoryStats",
                                      Domain::GetMemoryStats);
        NODE_SET_PROTOTYPE_METHOD(t, "blockPeek",
                                      Domain::BlockPeek);
        NODE_SET_PROTOTYPE_METHOD(t, "getBlockStats",
                                      Domain::GetBlockStats);
        NODE_SET_PROTOTYPE_METHOD(t, "getBlockInfo",
                                      Domain::GetBlockInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "getUUID",
                                      Domain::GetUUID);
        NODE_SET_PROTOTYPE_METHOD(t, "getVcpus",
                                      Domain::GetVcpus);
        NODE_SET_PROTOTYPE_METHOD(t, "setVcpus",
                                      Domain::SetVcpus);
        NODE_SET_PROTOTYPE_METHOD(t, "isActive",
                                      Domain::IsActive);
        NODE_SET_PROTOTYPE_METHOD(t, "isPersistent",
                                      Domain::IsPersistent);
        NODE_SET_PROTOTYPE_METHOD(t, "isUpdated",
                                      Domain::IsUpdated);
        NODE_SET_PROTOTYPE_METHOD(t, "getInterfaceStats",
                                      Domain::GetInterfaceStats);
        NODE_SET_PROTOTYPE_METHOD(t, "coreDump",
                                      Domain::CoreDump);
        NODE_SET_PROTOTYPE_METHOD(t, "migrate",
                                      Domain::Migrate);
        NODE_SET_PROTOTYPE_METHOD(t, "setMigrationMaxDowntime",
                                      Domain::SetMigrationMaxDowntime);
        NODE_SET_PROTOTYPE_METHOD(t, "pinVcpu",
                                      Domain::PinVcpu);
        NODE_SET_PROTOTYPE_METHOD(t, "reboot",
                                      Domain::Reboot);
        NODE_SET_PROTOTYPE_METHOD(t, "reset",
                                      Domain::Reset);
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
        NODE_SET_PROTOTYPE_METHOD(t, "sendKey",
                                      Domain::SendKey);
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
        NODE_SET_PROTOTYPE_METHOD(t, "hasCurrentSnapshot",
                                      Domain::HasCurrentSnapshot);
        NODE_SET_PROTOTYPE_METHOD(t, "revertToSnapshot",
                                      Domain::RevertToSnapshot);
        NODE_SET_PROTOTYPE_METHOD(t, "takeSnapshot",
                                      Domain::TakeSnapshot);
        NODE_SET_PROTOTYPE_METHOD(t, "getCurrentSnapshot",
                                      Domain::GetCurrentSnapshot);
        NODE_SET_PROTOTYPE_METHOD(t, "deleteSnapshot",
                                      Domain::DeleteSnapshot);
        NODE_SET_PROTOTYPE_METHOD(t, "lookupSnapshotByName",
                                      Domain::LookupSnapshotByName);
        NODE_SET_PROTOTYPE_METHOD(t, "getSnapshots",
                                      Domain::GetSnapshots);

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

#ifdef VIR_DOMAIN_PMSUSPENDED
        // If its available in libvirt.h, then make it available in node
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_PMSUSPENDED);
#endif

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

        //virDomainJobType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_NONE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_BOUNDED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_UNBOUNDED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_COMPLETED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_FAILED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_CANCELLED);

        //virDomainMemoryFlags
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MEMORY_VIRTUAL);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_MEMORY_PHYSICAL);

        //virDomainEventType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_DEFINED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_UNDEFINED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_RESUMED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED);

        //virDomainEventIOErrorAction
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_IO_ERROR_NONE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_IO_ERROR_PAUSE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_IO_ERROR_REPORT);

        //virDomainEventResumedDetailType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_RESUMED_UNPAUSED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_RESUMED_MIGRATED);

        //virDomainEventStartedDetailType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_BOOTED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_MIGRATED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_RESTORED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_FROM_SNAPSHOT);

        //virDomainEventStoppedDetailType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_SHUTDOWN);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_DESTROYED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_CRASHED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_MIGRATED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_SAVED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_FAILED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_FROM_SNAPSHOT);

        //virDomainEventSuspendedDetailType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_PAUSED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_MIGRATED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_IOERROR);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_WATCHDOG);

        //virDomainEventUndefinedDetailType
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_UNDEFINED_REMOVED);

        //virDomainEventWatchdogAction
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_NONE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_PAUSE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_RESET);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_POWEROFF);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_SHUTDOWN);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_DEBUG);


        NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SEND_KEY_MAX_KEYS);

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

        type_symbol         = NODE_PSYMBOL("type");
        time_symbol         = NODE_PSYMBOL("time");
        data_symbol         = NODE_PSYMBOL("data");
        file_symbol         = NODE_PSYMBOL("file");
        elapsed_symbol      = NODE_PSYMBOL("elapsed");
        remaining_symbol    = NODE_PSYMBOL("remaining");
        total_symbol        = NODE_PSYMBOL("total");
        processed_symbol    = NODE_PSYMBOL("processed");

        label_symbol = NODE_PSYMBOL("label");
        enforcing_symbol = NODE_PSYMBOL("enforcing");

        memory_stat_swap_in_symbol = NODE_PSYMBOL("swap_in");
        memory_stat_swap_out_symbol = NODE_PSYMBOL("swap_out");
        memory_stat_major_fault_symbol = NODE_PSYMBOL("major_fault");
        memory_stat_minor_fault_symbol = NODE_PSYMBOL("minor_fault");
        memory_stat_unused_symbol = NODE_PSYMBOL("unused");
        memory_stat_available_symbol = NODE_PSYMBOL("available");

        block_stat_rd_req_symbol = NODE_PSYMBOL("read_requests");
        block_stat_rd_bytes_symbol = NODE_PSYMBOL("read_bytes");
        block_stat_wr_req_symbol = NODE_PSYMBOL("write_requests");
        block_stat_wr_bytes_symbol = NODE_PSYMBOL("write_bytes");
        block_stat_errs_symbol = NODE_PSYMBOL("errors");

        block_info_capacity_symbol = NODE_PSYMBOL("capacity");
        block_info_allocation_symbol = NODE_PSYMBOL("allocation");
        block_info_physical_symbol = NODE_PSYMBOL("physical");

        nwiface_stat_rx_bytes_symbol = NODE_PSYMBOL("rx_bytes");
        nwiface_stat_rx_packets_symbol = NODE_PSYMBOL("rx_packets");
        nwiface_stat_rx_errors_symbol = NODE_PSYMBOL("rx_errors");
        nwiface_stat_rx_drop_symbol = NODE_PSYMBOL("rx_drop");
        nwiface_stat_tx_bytes_symbol = NODE_PSYMBOL("tx_bytes");
        nwiface_stat_tx_packets_symbol = NODE_PSYMBOL("tx_packets");
        nwiface_stat_tx_errors_symbol = NODE_PSYMBOL("tx_errors");
        nwiface_stat_tx_drop_symbol = NODE_PSYMBOL("tx_drop");
    }

    virDomainPtr Domain::domain() const {
        return domain_;
    }

    void DomainCreateWorker::Execute() {
        domainptr_ = virDomainCreateXML(getConnection(), xml_.c_str(), flags_);

        if (domainptr_ == NULL) {
            setVirError(virGetLastError());
        }
    }

    void DomainCreateWorker::HandleOKCallback() {
        NanScope();

        Domain *domain = new Domain();
        domain->domain_ = domainptr_;

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        Local<Value> argv[] = { NanNull(), domain_obj };

        callback->Call(2, argv);
    }

    NAN_METHOD(Domain::Create) {
        HandleScope scope;
        unsigned int flags = 0;

        int argsl = args.Length();

        if(argsl < 2) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at least two argument")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        if(!args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }
        String::Utf8Value xml(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainCreateWorker(callback, hypervisor->connection(), *xml, flags));

        NanReturnUndefined();
    }

    void DomainDefineWorker::Execute() {
        domainptr_ = virDomainDefineXML(getConnection(), xml_.c_str());

        if (domainptr_ == NULL) {
            setVirError(virGetLastError());
        }
    }

    void DomainDefineWorker::HandleOKCallback() {
        NanScope();

        Domain *domain = new Domain();
        domain->domain_ = domainptr_;

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        Local<Value> argv[] = { NanNull(), domain_obj };

        callback->Call(2, argv);
    }

    NAN_METHOD(Domain::Define) {
        HandleScope scope;
        unsigned int flags = 0;

        int argsl = args.Length();

        if(argsl < 2) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at least two argument")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        if(!args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }
        String::Utf8Value xml(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainDefineWorker(callback, hypervisor->connection(), *xml, flags));

        NanReturnUndefined();
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

    void DomainLookupByIdWorker::Execute() {
        domainptr_ = virDomainLookupByID(getConnection(), id_);

        if(domainptr_ == NULL) {
            setVirError(virGetLastError());
            return;
        }
    }

    void DomainLookupByIdWorker::HandleOKCallback() {
        NanScope();

        Domain *domain = new Domain();
        domain->domain_ = domainptr_;

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        Local<Value> argv[] = { NanNull(), domain_obj };

        callback->Call(2, argv);
    }

    NAN_METHOD(Domain::LookupById) {
        NanScope();

        int id = -1;

        if(args.Length() < 1 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid Domain Id.")));
        }

        if(args.Length() >= 2 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        id = args[0]->Int32Value();
        String::Utf8Value name(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainLookupByIdWorker(callback, hypervisor->connection(), id));

        NanReturnUndefined();
    }

    void DomainLookupByNameWorker::Execute() {
        domainptr_ = virDomainLookupByName(getConnection(), name_.c_str());

        if(domainptr_ == NULL) {
            setVirError(virGetLastError());
            return;
        }
    }

    void DomainLookupByNameWorker::HandleOKCallback() {
        NanScope();

        Domain *domain = new Domain();
        domain->domain_ = domainptr_;

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
        domain->Wrap(domain_obj);

        Local<Value> argv[] = { NanNull(), domain_obj };

        callback->Call(2, argv);
    }

    NAN_METHOD(Domain::LookupByName) {
        NanScope();

        if(args.Length() < 1 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid Domain name.")));
        }

        if(args.Length() >= 2 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value name(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainLookupByNameWorker(callback, hypervisor->connection(), (char*)*name));

        NanReturnUndefined();
    }

    Handle<Value> Domain::LookupByUUID(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a UUID string.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value uuid(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Domain *domain = new Domain();
        domain->domain_ = virDomainLookupByUUIDString(hypervisor->connection(), (const char *) *uuid);

        if(domain->domain_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();

        domain->Wrap(domain_obj);

        return scope.Close(domain_obj);
    }

    void DomainGetIdWorker::Execute() {
        unsigned int id = -1;

        id = virDomainGetID(getDomainPtr());

        if(id == -1u) {
            setVirError(virGetLastError());
            return;
        }

        setVal((int)id);
    }

    NOARGS_WORKER_METHOD(GetId, DomainGetIdWorker)

    void DomainGetInfoWorker::Execute() {
        int ret = -1;

        info_ = (virDomainInfoPtr)malloc(sizeof(virDomainInfo));

        if (info_ == NULL) {
            SetErrorMessage("unable to allocate memory");
            return;
        }

        ret = virDomainGetInfo(getDomainPtr(), info_);

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    void DomainGetInfoWorker::HandleOKCallback() {
        NanScope();

        Local<Object> object = NanNew<Object>();

        object->Set(state_symbol, NanNew<Integer>(info_->state)); //virDomainState
        object->Set(max_memory_symbol, NanNew<Number>(info_->maxMem)); //KBytes
        object->Set(memory_symbol, NanNew<Number>(info_->memory)); //KBytes
        object->Set(vcpus_number_symbol, NanNew<Integer>(info_->nrVirtCpu));
        object->Set(cpu_time_symbol, NanNew<Number>(info_->cpuTime)); //nanoseconds

        free(info_);

        Local<Value> argv[] = { NanNull(), object };

        callback->Call(2, argv);
    }

    NOARGS_WORKER_METHOD(GetInfo, DomainGetInfoWorker)

    void DomainGetNameWorker::Execute() {

      setVal((char*)virDomainGetName(getDomainPtr()));

      if(getVal() == NULL) {
        setVirError(virGetLastError());
      }
    }

    NOARGS_WORKER_METHOD(GetName, DomainGetNameWorker)

    void DomainGetUUIDWorker::Execute() {
      int ret = -1;
      char *uuid = new char[VIR_UUID_STRING_BUFLEN];

      ret = virDomainGetUUIDString(getDomainPtr(), uuid);

      if(ret == -1) {
        setVirError(virGetLastError());
        return;
      }

      setVal(uuid);
    }

    NOARGS_WORKER_METHOD(GetUUID, DomainGetUUIDWorker)

    void DomainGetAutostartWorker::Execute() {
        int ret = -1;
        int autostart_;

        ret = virDomainGetAutostart(getDomainPtr(), &autostart_);

        if(ret == -1) {
          setVirError(virGetLastError());
          return;
        }

        setVal(autostart_ == 0 ? false : true);
    }

    NOARGS_WORKER_METHOD(GetAutostart, DomainGetAutostartWorker)

    void DomainSetAutostartWorker::Execute() {
        int ret = -1;

        ret = virDomainSetAutostart(getDomainPtr(), autostart_);

        if(ret == -1) {
          setVirError(virGetLastError());
          return;
        }

        setVal(autostart_ == 0 ? false : true);
    }

    NAN_METHOD(Domain::SetAutostart) {
        NanScope();

        if(args.Length() < 1 || !args[0]->IsBoolean()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a boolean as first argument")));
        }

        if (args.Length() > 1 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        bool autostart = args[0]->IsTrue();

        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainSetAutostartWorker(callback, domain->domain_, autostart ? 1 : 0));

        NanReturnUndefined();
    }

    void DomainGetOsTypeWorker::Execute() {

      setVal(virDomainGetOSType(getDomainPtr()));

      if(getVal() == NULL) {
        setVirError(virGetLastError());
      }
    }

    NOARGS_WORKER_METHOD(GetOsType, DomainGetOsTypeWorker)

    void DomainGetMaxMemoryWorker::Execute() {

      setVal(virDomainGetMaxMemory(getDomainPtr()));

      if(getVal() == 0) {
        setVirError(virGetLastError());
      }
    }

    NOARGS_WORKER_METHOD(GetMaxMemory, DomainGetMaxMemoryWorker)

    void DomainSetMaxMemoryWorker::Execute() {

        int ret = -1;

        ret = virDomainSetMaxMemory(getDomainPtr(), maxmem_);

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NAN_METHOD(Domain::SetMaxMemory) {
        NanScope();

        if(args.Length() < 1 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a number as first argument")));
        }

        if (args.Length() > 1 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainSetMaxMemoryWorker(callback, domain->domain_, args[0]->Int32Value()));

        NanReturnUndefined();
    }

    void DomainSetMemoryWorker::Execute() {

        int ret = -1;

        ret = virDomainSetMemory(getDomainPtr(), mem_);

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NAN_METHOD(Domain::SetMemory) {
        NanScope();

        if(args.Length() < 1 || !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid amount of memory")));
        }

        if (args.Length() > 1 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainSetMemoryWorker(callback, domain->domain_, args[0]->Int32Value()));

        NanReturnUndefined();
    }

    void DomainGetMaxVcpusWorker::Execute() {

        setVal(virDomainGetMaxVcpus(getDomainPtr()));

        if(getVal() == -1) {
            setVirError(virGetLastError());
        }
    }

    NOARGS_WORKER_METHOD(GetMaxVcpus, DomainGetMaxVcpusWorker)

    void DomainIsActiveWorker::Execute() {
        int ret = -1;

        ret = virDomainIsActive(getDomainPtr());

        if(ret == -1) {
            setVirError(virGetLastError());
            return;
        }

        setVal(ret == 1 ? true : false);
    }

    NOARGS_WORKER_METHOD(IsPersistent, DomainIsPersistentWorker)

    void DomainIsPersistentWorker::Execute() {
        int ret = -1;

        ret = virDomainIsPersistent(getDomainPtr());

        if(ret == -1) {
            setVirError(virGetLastError());
            return;
        }

        setVal(ret == 1 ? true : false);
    }

    NOARGS_WORKER_METHOD(IsActive, DomainIsActiveWorker)

    void DomainIsUpdatedWorker::Execute() {
        int ret = -1;

        ret = virDomainIsUpdated(getDomainPtr());

        if(ret == -1) {
            setVirError(virGetLastError());
            return;
        }

        setVal(ret == 1 ? true : false);
    }

    NOARGS_WORKER_METHOD(IsUpdated, DomainIsUpdatedWorker)

    void DomainRebootWorker::Execute() {
        int ret = -1;

        ret = virDomainReboot(getDomainPtr(), 0);

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NOARGS_WORKER_METHOD(Reboot, DomainRebootWorker)

    void DomainResetWorker::Execute() {
        int ret = -1;

        ret = virDomainReset(getDomainPtr(), 0);

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NOARGS_WORKER_METHOD(Reset, DomainResetWorker)

    void DomainSaveWorker::Execute() {
        int ret = -1;

        ret = virDomainSave(getDomainPtr(), path_.c_str());

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NAN_METHOD(Domain::Save) {
        HandleScope scope;

        if(args.Length() < 1 && !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        if(args.Length() > 1 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        String::Utf8Value path(args[0]->ToString());

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainSaveWorker(callback, domain->domain_, *path));

        NanReturnUndefined();
    }

    void DomainRestoreWorker::Execute() {
        int ret = -1;

        ret = virDomainRestore(getConnection(), path_.c_str());

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NAN_METHOD(Domain::Restore) {
        HandleScope scope;

        if(args.Length() < 1 && !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        if(args.Length() > 1 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }

        String::Utf8Value path(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainRestoreWorker(callback, hypervisor->connection(), *path));

        NanReturnUndefined();
    }

    void DomainSuspendWorker::Execute() {
        int ret = -1;

        ret = virDomainSuspend(getDomainPtr());

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NOARGS_WORKER_METHOD(Suspend, DomainSuspendWorker)

    void DomainResumeWorker::Execute() {
        int ret = -1;

        ret = virDomainResume(getDomainPtr());

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NOARGS_WORKER_METHOD(Resume, DomainResumeWorker)

    void DomainShutdownWorker::Execute() {
        int ret = -1;

        ret = virDomainShutdown(getDomainPtr());

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NOARGS_WORKER_METHOD(Shutdown, DomainShutdownWorker)

    void DomainStartWorker::Execute() {
        int ret = -1;

        ret = virDomainCreate(getDomainPtr());

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NOARGS_WORKER_METHOD(Start, DomainStartWorker)

    Handle<Value> Domain::SendKey(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        if(args.Length() == 0) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify arguments to invoke this function")));
        }

        if(!args[0]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("Argument must be an array of objects")));
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        unsigned int keycodes[VIR_DOMAIN_SEND_KEY_MAX_KEYS];

        Local<Array> keycodes_ = Local<Array>::Cast(args[0]);

        unsigned int length = keycodes_->Length();

        for(unsigned int i = 0; i < length; i++) {
            keycodes[i] = (unsigned int) keycodes_->Get(Integer::New(i))->Int32Value();
        }

        ret = virDomainSendKey(domain->domain_, 0, 150, keycodes, length, 0);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }
        return True();
    }

    void DomainGetVcpusWorker::Execute() {
        int ncpus;

        if(virDomainGetInfo(getDomainPtr(), &info_) == -1) {
            setVirError(virGetLastError());
            return;
        }

        if(virNodeGetInfo(getConnection(), &nodeinfo_) == -1) {
            setVirError(virGetLastError());
            return;
        }

        cpuinfo_.resize(info_.nrVirtCpu);
        //memset(cpuinfo_.data(), 0, sizeof(*cpuinfo_) * info_.nrVirtCpu);

        cpumaplen_ = VIR_CPU_MAPLEN(VIR_NODEINFO_MAXCPUS(nodeinfo_));

        cpumap_.resize(info_.nrVirtCpu * cpumaplen_, 0);

        ncpus = virDomainGetVcpus(getDomainPtr(), cpuinfo_.data(), info_.nrVirtCpu, cpumap_.data(), cpumaplen_);

        if(ncpus < 0) {
            setVirError(virGetLastError());
        }
    }

    void DomainGetVcpusWorker::HandleOKCallback() {
        NanScope();

        Local<Array> vcpus = Array::New(info_.nrVirtCpu);

        for(int i = 0; i < info_.nrVirtCpu; i++) {
            Local<Object> obj = NanNew<Object>();
            obj->Set(number_symbol, NanNew<Integer>(cpuinfo_[i].number));
            obj->Set(state_symbol, NanNew<Integer>(cpuinfo_[i].state));
            obj->Set(cpu_time_symbol, NanNew<Number>(cpuinfo_[i].cpuTime)); //nanoseconds
            obj->Set(cpu_symbol, NanNew<Integer>(cpuinfo_[i].cpu));

            int maxcpus =  VIR_NODEINFO_MAXCPUS(nodeinfo_);
            Local<Array> affinity = NanNew<Array>(maxcpus);
            for(int j = 0; j < maxcpus; j++) {
                Local<Object> cpu = NanNew<Object>();
                cpu->Set(cpu_symbol, NanNew<Integer>(j));
                cpu->Set(usable_symbol, NanNew<Boolean>(VIR_CPU_USABLE(cpumap_, cpumaplen_, i, j)));

                affinity->Set(NanNew<Integer>(j), cpu);
            }
            obj->Set(affinity_symbol, affinity);

            vcpus->Set(NanNew<Integer>(i), obj);
        }

        Local<Value> argv[] = { NanNull(), vcpus };

        callback->Call(2, argv);
    }

    NOARGS_WORKER_METHOD(GetVcpus, DomainGetVcpusWorker)

    void DomainSetVcpusWorker::Execute() {
        int ret = -1;

        ret = virDomainSetVcpus(getDomainPtr(), vcpus_);

        if(ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NAN_METHOD(Domain::SetVcpus) {
        HandleScope scope;

        if(args.Length() < 1 && !args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a number as first argument")));
        }

        if(args.Length() > 1 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainSetVcpusWorker(callback, domain->domain_, args[0]->Int32Value()));

        NanReturnUndefined();
    }

    Handle<Value> Domain::Migrate(const Arguments& args) {
        HandleScope scope;
        unsigned long flags = 0;
        unsigned long bandwidth = 0;
//        const char* dest_name = NULL;
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
        String::Utf8Value dest_uri(args_->Get(migration_uri_symbol));

        //dest_name
        //if(args_->Has(migration_name_symbol)) {
            String::Utf8Value dest_name(args_->Get(migration_name_symbol));
            //dest_name = ToCString(dest_name_);
        //}

        //flags
        if(args_->Has(migration_flags_symbol)){
            Local<Array> flags_ = Local<Array>::Cast(args_->Get(migration_flags_symbol));
            unsigned int length = flags_->Length();

            for (unsigned int i = 0; i < length; i++) {
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
                                                        (const char *) *dest_name,
                                                        (const char *) *dest_uri,
                                                        bandwidth);

            if(migrated_domain->domain_ == NULL) {
                ThrowException(Error::New(virGetLastError()));
                return False();
            }

            migrated_domain->Wrap(args.This());

            return scope.Close(migrated_domain->constructor_template->GetFunction()->NewInstance());
        } else {
            ret = virDomainMigrateToURI(domain->domain_,
                                        (const char *) *dest_uri,
                                        flags,
                                        (const char *) *dest_name,
                                        bandwidth);
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

    DomainPinVcpuWorker::DomainPinVcpuWorker(NanCallback *callback, virDomainPtr domainptr, int vcpu, Handle<Array> cpus)
        : DomainWorker(callback, domainptr), vcpu_(vcpu) {
            NanScope();

            length_ = cpus->Length();
            usables_.resize(length_);
            vcpus_.resize(length_);

            for(int i = 0; i < length_; i++) {

                if(!cpus->Get(NanNew<Integer>(i))->IsObject()) {
                    ThrowException(Exception::TypeError(
                        String::New("The second argument must be an array of objects")));
                }

                Local<Object> cpu = cpus->Get(NanNew<Integer>(i))->ToObject();

                usables_[i] = cpu->Get(usable_symbol)->IsTrue();
                vcpus_[i] = cpu->Get(cpu_symbol)->Int32Value();
            }
    }

    void DomainPinVcpuWorker::Execute() {
        virNodeInfo nodeinfo;
        int maxcpus;
        int cpumaplen;

        if(virNodeGetInfo(getConnection(), &nodeinfo) == -1) {
            setVirError(virGetLastError());
            return;
        }

        maxcpus = VIR_NODEINFO_MAXCPUS(nodeinfo);
        cpumaplen = VIR_CPU_MAPLEN(maxcpus);

        std::vector<unsigned char> cpumap(cpumaplen);

        for(int i = 0; i < length_; i++) {
            if(i > maxcpus) {
                break;
            }

            if(usables_[i]) {
                VIR_USE_CPU(cpumap.data(), vcpus_[i]);
            } else {
                VIR_UNUSE_CPU(cpumap.data(), vcpus_[i]);
            }
        }

        if(virDomainPinVcpu(getDomainPtr(), vcpu_, cpumap.data(), cpumaplen) == -1) {
            setVirError(virGetLastError());
            return;
        }
    }

    NAN_METHOD(Domain::PinVcpu) {
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

        if(args.Length() > 2 && !args[2]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("The third argument must be a function")));
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        Local<Array> cpus = Local<Array>::Cast(args[1]);

        NanCallback *callback = new NanCallback(args[2].As<Function>());

        NanAsyncQueueWorker(new DomainPinVcpuWorker(callback, domain->domain_, args[0]->Int32Value(), cpus));

        NanReturnUndefined();

    }

    void DomainAttachDeviceWorker::Execute() {
        int ret = -1;

        if(flags_ > 0) {
            ret = virDomainAttachDeviceFlags(getDomainPtr(), xml_.c_str(), flags_);
        } else {
            ret = virDomainAttachDevice(getDomainPtr(), xml_.c_str());
        }

        if (ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NAN_METHOD(Domain::AttachDevice) {
        HandleScope scope;
        unsigned int flags = 0;

        int argsl = args.Length();

        if(argsl < 1 || argsl > 2) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at two or three arguments")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("The first argument must be an integer")));
        }

        if(argsl == 2 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("The second argument must be an function")));
        }

        if(argsl == 3 && !args[1]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an object with flags")));
        }

        if(argsl == 3 && !args[2]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("The third argument must be an function")));
        }

        String::Utf8Value xml(args[0]->ToString());

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[1]);
        unsigned int length = flags_->Length();

        for (unsigned int i = 0; i < length; i++) {
            flags |= flags_->Get(NanNew<Integer>(i))->Int32Value();
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        NanCallback *callback = new NanCallback(args[argsl == 3 ? 2 : 1].As<Function>());

        NanAsyncQueueWorker(new DomainAttachDeviceWorker(callback, domain->domain_, *xml, flags));

        NanReturnUndefined();
    }

    void DomainDetachDeviceWorker::Execute() {
        int ret = -1;

        if(flags_ > 0) {
            ret = virDomainDetachDeviceFlags(getDomainPtr(), xml_.c_str(), flags_);
        } else {
            ret = virDomainDetachDevice(getDomainPtr(), xml_.c_str());
        }

        if (ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NAN_METHOD(Domain::DetachDevice) {
        HandleScope scope;
        unsigned int flags = 0;

        int argsl = args.Length();

        if(argsl < 1 || argsl > 2) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at two or three arguments")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("The first argument must be an integer")));
        }

        if(argsl == 2 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("The second argument must be an function")));
        }

        if(argsl == 3 && !args[1]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an object with flags")));
        }

        if(argsl == 3 && !args[2]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("The third argument must be an function")));
        }

        String::Utf8Value xml(args[0]->ToString());

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[1]);
        unsigned int length = flags_->Length();

        for (unsigned int i = 0; i < length; i++) {
            flags |= flags_->Get(NanNew<Integer>(i))->Int32Value();
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        NanCallback *callback = new NanCallback(args[argsl == 3 ? 2 : 1].As<Function>());

        NanAsyncQueueWorker(new DomainDetachDeviceWorker(callback, domain->domain_, *xml, flags));

        NanReturnUndefined();
    }

    void DomainUpdateDeviceWorker::Execute() {
        int ret = -1;

        ret = virDomainUpdateDeviceFlags(getDomainPtr(), xml_.c_str(), flags_);

        if (ret == -1) {
            setVirError(virGetLastError());
        }
    }

    NAN_METHOD(Domain::UpdateDevice) {
        HandleScope scope;
        unsigned int flags = 0;

        int argsl = args.Length();

        if(argsl != 3) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify three arguments")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("The first argument must be an integer")));
        }

        if(!args[1]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an object with flags")));
        }

        if(!args[2]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("The third argument must be an function")));
        }

        String::Utf8Value xml(args[0]->ToString());

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[1]);
        unsigned int length = flags_->Length();

        for (unsigned int i = 0; i < length; i++) {
            flags |= flags_->Get(NanNew<Integer>(i))->Int32Value();
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        NanCallback *callback = new NanCallback(args[2].As<Function>());

        NanAsyncQueueWorker(new DomainUpdateDeviceWorker(callback, domain->domain_, *xml, flags));

        NanReturnUndefined();
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
        return True();
    }

    void DomainToXmlWorker::Execute() {

      setVal(virDomainGetXMLDesc(getDomainPtr(), flags_));

      if(getVal() == NULL) {
        setVirError(virGetLastError());
      }
    }

    NAN_METHOD(Domain::ToXml) {
        NanScope();

        int flags = 0;

        if(args.Length() < 1 && !args[0]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an array as first argument to invoke this function")));
        }

        if(args.Length() < 2 && !args[1]->IsFunction()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a function as second argument")));
        }

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[0]);
        unsigned int length = flags_->Length();

        for (unsigned int i = 0; i < length; i++) {
            flags |= flags_->Get(NanNew<Integer>(i))->Int32Value();
        }

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        NanCallback *callback = new NanCallback(args[1].As<Function>());

        NanAsyncQueueWorker(new DomainToXmlWorker(callback, domain->domain_, flags));

        NanReturnUndefined();
    }

    Handle<Value> Domain::GetJobInfo(const Arguments& args) {
        HandleScope scope;
        virDomainJobInfo info_;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainGetJobInfo(domain->domain_, &info_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> info = Object::New();
        info->Set(type_symbol, Integer::New(info_.type));

        //time
        Local<Object> time = Object::New();
        time->Set(elapsed_symbol, Number::New(info_.timeElapsed));
        time->Set(remaining_symbol, Number::New(info_.timeRemaining));

        //data
        Local<Object> data = Object::New();
        data->Set(total_symbol, Number::New(info_.dataTotal));
        data->Set(processed_symbol, Number::New(info_.dataProcessed));
        data->Set(remaining_symbol, Number::New(info_.dataRemaining));

        //memory
        Local<Object> memory = Object::New();
        memory->Set(total_symbol, Number::New(info_.memTotal));
        memory->Set(processed_symbol, Number::New(info_.memProcessed));
        memory->Set(remaining_symbol, Number::New(info_.memRemaining));

        //file
        Local<Object> file = Object::New();
        file->Set(total_symbol, Number::New(info_.fileTotal));
        file->Set(processed_symbol, Number::New(info_.fileProcessed));
        file->Set(remaining_symbol, Number::New(info_.fileRemaining));

        info->Set(time_symbol, time);
        info->Set(data_symbol, data);
        info->Set(memory_symbol, memory);
        info->Set(file_symbol, file);

        return scope.Close(info);
    }

    Handle<Value> Domain::AbortCurrentJob(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        ret = virDomainAbortJob(domain->domain_);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::GetSchedType(const Arguments& args) {
        HandleScope scope;
        return Undefined();
    }

    Handle<Value> Domain::GetSchedParams(const Arguments& args) {
        HandleScope scope;
        int nparams = 0;
        int ret = -1;
        char *type = NULL;
        virSchedParameterPtr params_;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        type = virDomainGetSchedulerType(domain->domain_, &nparams);

        if(type == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        free(type);

        params_ = (virSchedParameterPtr) malloc(sizeof(*params_) * nparams);

        if(params_ == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return Null();
        }
        memset(params_, 0, sizeof(*params_) * nparams);

        ret = virDomainGetSchedulerParameters(domain->domain_, params_, &nparams);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            free(params_);
            return Null();
        }

        Local<Object> params = Object::New();

        for(int i = 0; i < nparams; i++) {
            Local<Value> value = Local<Value>::New(Null());

            switch(params_[i].type) {
                case VIR_DOMAIN_SCHED_FIELD_INT:
                    value = Integer::New(params_[i].value.i);
                    break;
                case VIR_DOMAIN_SCHED_FIELD_UINT:
                    value = Integer::New(params_[i].value.ui);
                    break;
                case VIR_DOMAIN_SCHED_FIELD_LLONG:
                    value = Number::New(params_[i].value.l);
                    break;
                case VIR_DOMAIN_SCHED_FIELD_ULLONG:
                    value = Number::New(params_[i].value.ul);
                    break;
                case VIR_DOMAIN_SCHED_FIELD_DOUBLE:
                    value = Number::New(params_[i].value.d);
                    break;
                case VIR_DOMAIN_SCHED_FIELD_BOOLEAN:
                    value = Integer::New(params_[i].value.b);
                    break;
            }

            params->Set(String::New(params_[i].field), value);
        }
        free(params_);

        return scope.Close(params);
    }

    Handle<Value> Domain::SetSchedParams(const Arguments& args) {
        HandleScope scope;
        virSchedParameterPtr params = NULL;
        int nparams = 0;
        char *type = NULL;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsObject()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an object as argument to invoke this function")));
        }

        Local<Object> newparams = args[0]->ToObject();

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        type = virDomainGetSchedulerType(domain->domain_, &nparams);

        if(type == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }
        free(type);

        params = (virSchedParameterPtr) malloc(sizeof(*params) * nparams);
        if(params == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return False();
        }

        memset(params, 0, sizeof(*params) * nparams);

        ret = virDomainGetSchedulerParameters(domain->domain_, params, &nparams);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            free(params);
            return False();
        }

        for(int i = 0; i < nparams; i++) {
            Local<String> field = String::New(params[i].field);
            if(!newparams->Has(field)) {
                continue;
            }

            Local<Value> value = newparams->Get(field);

            switch (params[i].type) {
                case VIR_DOMAIN_SCHED_FIELD_INT:
                    params[i].value.i = value->Int32Value();
                    break;
                case VIR_DOMAIN_SCHED_FIELD_UINT:
                    params[i].value.ui = value->Uint32Value();
                    break;
                case VIR_DOMAIN_SCHED_FIELD_LLONG:
                    params[i].value.l = value->NumberValue();
                    break;
                case VIR_DOMAIN_SCHED_FIELD_ULLONG:
                    params[i].value.ul = value->NumberValue();
                    break;
                case VIR_DOMAIN_SCHED_FIELD_DOUBLE:
                    params[i].value.d = value->NumberValue();
                    break;
                case VIR_DOMAIN_SCHED_FIELD_BOOLEAN:
                    params[i].value.b = value->Uint32Value();
                    break;
            }
        }
        ret = virDomainSetSchedulerParameters(domain->domain_, params, nparams);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            free(params);
            return False();
        }
        free(params);

        return True();
    }

    Handle<Value> Domain::GetSecurityLabel(const Arguments& args) {
        HandleScope scope;
        virSecurityLabel label_;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        memset(&label_, 0, sizeof label_);
        ret = virDomainGetSecurityLabel(domain->domain_, &label_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> label = Object::New();
        label->Set(label_symbol, String::New(label_.label));
        label->Set(enforcing_symbol, Boolean::New(label_.enforcing));

        return scope.Close(label);
    }

    Handle<Value> Domain::SaveManagedImage(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainManagedSave(domain->domain_, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::RemoveManagedImage(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainManagedSaveRemove(domain->domain_, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::HasManagedImage(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainHasManagedSaveImage(domain->domain_, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return scope.Close(Boolean::New(ret));
    }

    Handle<Value> Domain::MemoryPeek(const Arguments& args) {
        HandleScope scope;
        unsigned long long start = 0;
		size_t size = 0;
		char * buffer_ = NULL;
		unsigned int flags = 0;

        if(args.Length() < 3) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify three arguments to invoke this function")));
        }

        if(!args[0]->IsNumber() || !args[1]->IsNumber()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a number in the first and second argument")));
        }

        if(!args[2]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an array in the third argument")));
        }

        start = args[0]->NumberValue();
        size = args[1]->NumberValue() * sizeof(char *);

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[2]);
        unsigned int length = flags_->Length();

        for (unsigned int i = 0; i < length; i++) {
            flags |= flags_->Get(Integer::New(i))->Int32Value();
        }

        buffer_ = (char*) malloc(size);

        if(buffer_ == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return Null();
        }

        memset(buffer_, 0, size);

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        int ret = virDomainMemoryPeek(domain->domain_, start, size, buffer_, flags);

        if(ret == -1) {
            free(buffer_);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Buffer *buffer = Buffer::New(size);
        memcpy(Buffer::Data(buffer), buffer_, size);
        free(buffer_);

        return scope.Close(buffer->handle_);
    }

    Handle<Value> Domain::GetMemoryStats(const Arguments& args) {
        HandleScope scope;
        unsigned int nr_stats = 0;
        unsigned int flags = 0;
        virDomainMemoryStatStruct stats_[VIR_DOMAIN_MEMORY_STAT_NR];

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        nr_stats = virDomainMemoryStats(domain->domain_, stats_,
                                    VIR_DOMAIN_MEMORY_STAT_NR, flags);

        if(nr_stats == -1u) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> stats = Object::New();

        for(unsigned int i = 0; i < nr_stats; i++) {
            switch (stats_[i].tag) {
                case VIR_DOMAIN_MEMORY_STAT_SWAP_IN:
                    stats->Set(memory_stat_swap_in_symbol, Number::New(stats_[i].val));
                    break;
                case VIR_DOMAIN_MEMORY_STAT_SWAP_OUT:
                    stats->Set(memory_stat_swap_out_symbol, Number::New(stats_[i].val));
                    break;
                case VIR_DOMAIN_MEMORY_STAT_MAJOR_FAULT:
                    stats->Set(memory_stat_major_fault_symbol, Number::New(stats_[i].val));
                    break;
                case VIR_DOMAIN_MEMORY_STAT_MINOR_FAULT:
                    stats->Set(memory_stat_minor_fault_symbol, Number::New(stats_[i].val));
                    break;
                case VIR_DOMAIN_MEMORY_STAT_UNUSED:
                    stats->Set(memory_stat_unused_symbol, Number::New(stats_[i].val));
                    break;
                case VIR_DOMAIN_MEMORY_STAT_AVAILABLE:
                    stats->Set(memory_stat_available_symbol, Number::New(stats_[i].val));
                    break;
            }
        }
        return scope.Close(stats);
    }

    Handle<Value> Domain::BlockPeek(const Arguments& args) {
        HandleScope scope;
        unsigned long long start = 0;
		size_t size = 0;
		char * buffer_ = NULL;
		unsigned int flags = 0;

        if(args.Length() < 3) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify three arguments to invoke this function")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string in the first and second argument")));
        }

        if(!args[1]->IsNumber() || !args[2]->IsNumber()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify numbers in the first and second argument")));
        }

        String::Utf8Value path(args[0]->ToString());

        start = args[1]->NumberValue();
        size = args[2]->NumberValue() * sizeof(char *);

        buffer_ = (char*) malloc(size);

        if(buffer_ == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return Null();
        }

        memset(buffer_, 0, size);

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        int ret = virDomainBlockPeek(domain->domain_, (const char *) *path, start, size, buffer_, flags);

        if(ret == -1) {
            free(buffer_);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Buffer *buffer = Buffer::New(size);
        memcpy(Buffer::Data(buffer), buffer_, size);
        free(buffer_);

        return scope.Close(buffer->handle_);
    }

    Handle<Value> Domain::GetBlockStats(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        virDomainBlockStatsStruct stats_;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to invoke this function")));
        }
        String::Utf8Value path(args[0]->ToString());

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainBlockStats(domain->domain_, (const char *) *path, &stats_, sizeof(stats_));

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> stats = Object::New();
        stats->Set(block_stat_rd_req_symbol, Number::New(stats_.rd_req));
        stats->Set(block_stat_rd_bytes_symbol, Number::New(stats_.rd_bytes));
        stats->Set(block_stat_wr_req_symbol, Number::New(stats_.wr_req));
        stats->Set(block_stat_wr_bytes_symbol, Number::New(stats_.wr_bytes));
        stats->Set(block_stat_errs_symbol, Number::New(stats_.errs));

        return scope.Close(stats);
    }

    Handle<Value> Domain::GetBlockInfo(const Arguments& args) {
        HandleScope scope;
        virDomainBlockInfo info_;
        unsigned int flags = 0;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to invoke this function")));
        }
        String::Utf8Value path(args[0]->ToString());

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainGetBlockInfo(domain->domain_, (const char *) *path, &info_, flags);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> info = Object::New();
        info->Set(block_info_capacity_symbol, Number::New(info_.capacity));
        info->Set(block_info_allocation_symbol, Number::New(info_.allocation));
        info->Set(block_info_physical_symbol, Number::New(info_.physical));

        return scope.Close(info);
    }

    Handle<Value> Domain::GetInterfaceStats(const Arguments& args) {
        HandleScope scope;
        struct _virDomainInterfaceStats stats_;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to invoke this function")));
        }
        String::Utf8Value device(args[0]->ToString());

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainInterfaceStats(domain->domain_, (const char *) *device, &stats_, sizeof(stats_));
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> stats = Object::New();
        stats->Set(nwiface_stat_rx_bytes_symbol, Number::New(stats_.rx_bytes));
        stats->Set(nwiface_stat_rx_packets_symbol, Number::New(stats_.rx_packets));
        stats->Set(nwiface_stat_rx_errors_symbol, Number::New(stats_.rx_errs));
        stats->Set(nwiface_stat_rx_drop_symbol, Number::New(stats_.rx_drop));
        stats->Set(nwiface_stat_tx_bytes_symbol, Number::New(stats_.tx_bytes));
        stats->Set(nwiface_stat_tx_packets_symbol, Number::New(stats_.tx_packets));
        stats->Set(nwiface_stat_tx_errors_symbol, Number::New(stats_.tx_errs));
        stats->Set(nwiface_stat_tx_drop_symbol, Number::New(stats_.tx_drop));

        return scope.Close(stats);
    }

    Handle<Value> Domain::CoreDump(const Arguments& args) {
        HandleScope scope;
        int flags = 0;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to invoke this function")));
        }
        String::Utf8Value path(args[0]->ToString());

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainCoreDump(domain->domain_, (const char *) *path, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Domain::HasCurrentSnapshot(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainHasCurrentSnapshot(domain->domain_, flags);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return ret == 1 ? True() : False();
    }

    Handle<Value> Domain::RevertToSnapshot(const Arguments& args) {
        HandleScope scope;
        virDomainSnapshotPtr snapshot = NULL;
        unsigned int flags = 0;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to invoke this function")));
        }
        String::Utf8Value name(args[0]->ToString());

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        snapshot = virDomainSnapshotLookupByName(domain->domain_, (const char *) *name, flags);
        if(snapshot == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        ret = virDomainRevertToSnapshot(snapshot, flags);

        if(ret == -1) {
            virDomainSnapshotFree(snapshot);
            ThrowException(Error::New(virGetLastError()));
            return False();
        }
        virDomainSnapshotFree(snapshot);

        return True();
    }

    Handle<Value> Domain::TakeSnapshot(const Arguments& args) {
        HandleScope scope;
        virDomainSnapshotPtr snapshot = NULL;
        unsigned int flags = 0;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        if(args.Length() == 1 && args[0]->IsString()) {
            String::Utf8Value xml(args[0]->ToString());
            snapshot = virDomainSnapshotCreateXML(domain->domain_, (const char *) *xml, flags);
        } else {
            snapshot = virDomainSnapshotCurrent(domain->domain_, flags);
        }

        if(snapshot == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }
        virDomainSnapshotFree(snapshot);

        return True();
    }

    Handle<Value> Domain::GetCurrentSnapshot(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        virDomainSnapshotPtr snapshot = NULL;
        char* xml_ = NULL;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        snapshot = virDomainSnapshotCurrent(domain->domain_, flags);

        if(snapshot == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        xml_ = virDomainSnapshotGetXMLDesc(snapshot, flags);
        if(xml_ == NULL) {
            virDomainSnapshotFree(snapshot);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        virDomainSnapshotFree(snapshot);
        //TODO serialize to json

        Local<String> xml = String::New(xml_);
        free(xml_);

        return scope.Close(xml);
    }

    Handle<Value> Domain::DeleteSnapshot(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        virDomainSnapshotPtr snapshot = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to invoke this function")));
        }
        String::Utf8Value name(args[0]->ToString());

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        snapshot = virDomainSnapshotLookupByName(domain->domain_, (const char *) *name, flags);
        if(snapshot == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        int ret = virDomainSnapshotDelete(snapshot, flags);
        if(ret == -1) {
            virDomainSnapshotFree(snapshot);
            ThrowException(Error::New(virGetLastError()));
            return False();
        }
        virDomainSnapshotFree(snapshot);

        return True();
    }

    Handle<Value> Domain::LookupSnapshotByName(const Arguments& args) {
        HandleScope scope;
        char* xml_ = NULL;
        unsigned int flags = 0;
        virDomainSnapshotPtr snapshot = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to invoke this function")));
        }
        String::Utf8Value name(args[0]->ToString());

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        snapshot = virDomainSnapshotLookupByName(domain->domain_, (const char *) *name, flags);
        if(snapshot == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        xml_ = virDomainSnapshotGetXMLDesc(snapshot, flags);
        if(xml_ == NULL) {
            virDomainSnapshotFree(snapshot);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        virDomainSnapshotFree(snapshot);
        //TODO serialize to json

        Local<String> xml = String::New(xml_);
        free(xml_);

        return scope.Close(xml);
    }

    Handle<Value> Domain::GetSnapshots(const Arguments& args) {
        HandleScope scope;
        char **snapshots_ = NULL;
        unsigned int flags = 0;
        int num_snapshots = 0;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        num_snapshots = virDomainSnapshotNum(domain->domain_, flags);

        if(num_snapshots == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        snapshots_ = (char**) malloc(sizeof(*snapshots_) * num_snapshots);
        if(snapshots_ == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return Null();
        }

        num_snapshots = virDomainSnapshotListNames(domain->domain_, snapshots_, num_snapshots, flags);
        if(num_snapshots == -1) {
            free(snapshots_);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Array> snapshots = Array::New(num_snapshots);
        for (int i = 0; i < num_snapshots; i++) {
            virDomainSnapshotPtr snapshot = virDomainSnapshotLookupByName(domain->domain_, snapshots_[i], flags);
            char *xml = virDomainSnapshotGetXMLDesc(snapshot, flags);
            virDomainSnapshotFree(snapshot);

            snapshots->Set(Integer::New(i), String::New(xml));
            free(snapshots_[i]);
            free(xml);
        }
        free(snapshots_);

        return scope.Close(snapshots);
    }

} //namespace NodeLibvirt
