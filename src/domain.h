// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_DOMAIN_H_
#define SRC_DOMAIN_H_

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"
#include "worker.h"

namespace NodeLibvirt {

    class Domain : public ObjectWrap {
        friend class Hypervisor;
        friend class LookupDomainByNameWorker;

        public:
            static void Initialize();
            static inline bool HasInstance(v8::Handle<v8::Value> value) {
                if (!value->IsObject()) {
                    return false;
                }
                v8::Local<v8::Object> object = value->ToObject();
                return constructor_template->HasInstance(object);
            }
            virtual ~Domain() {
                if (domain_ != NULL) {
                    virDomainFree(domain_);
                }
            }
            virDomainPtr domain() const;
        protected:
            static Handle<Value> Create(const Arguments& args);
            static Handle<Value> LookupById(const Arguments& args);
            static Handle<Value> LookupByName(const Arguments& args);
            static Handle<Value> LookupByUUID(const Arguments& args);
            static Handle<Value> Define(const Arguments& args);
            static Handle<Value> Undefine(const Arguments& args);
            static Handle<Value> GetId(const Arguments& args);
            static Handle<Value> GetInfo(const Arguments& args);
            static Handle<Value> GetName(const Arguments& args);
            static Handle<Value> GetUUID(const Arguments& args);
            static Handle<Value> GetAutostart(const Arguments& args);
            static Handle<Value> SetAutostart(const Arguments& args);
            static Handle<Value> GetOsType(const Arguments& args);
            static Handle<Value> GetMaxMemory(const Arguments& args);
            static Handle<Value> SetMaxMemory(const Arguments& args);
            static Handle<Value> SetMemory(const Arguments& args);
            static Handle<Value> GetMaxVcpus(const Arguments& args);
            static Handle<Value> IsActive(const Arguments& args);
            static Handle<Value> IsPersistent(const Arguments& args);
            static Handle<Value> IsUpdated(const Arguments& args);
            static Handle<Value> Reboot(const Arguments& args);
            static Handle<Value> Reset(const Arguments& args);
            static Handle<Value> Save(const Arguments& args);
            static Handle<Value> Restore(const Arguments& args);
            static Handle<Value> Suspend(const Arguments& args);
            static Handle<Value> Resume(const Arguments& args);
            static Handle<Value> Shutdown(const Arguments& args);
            static Handle<Value> Start(const Arguments& args);
            static Handle<Value> Destroy(const Arguments& args);
            static Handle<Value> SendKey(const Arguments& args);
            static Handle<Value> GetVcpus(const Arguments& args);
            static Handle<Value> SetVcpus(const Arguments& args);
            static Handle<Value> Migrate(const Arguments& args);
            static Handle<Value> SetMigrationMaxDowntime(const Arguments& args);
            static Handle<Value> PinVcpu(const Arguments& args);
            static Handle<Value> AttachDevice(const Arguments& args);
            static Handle<Value> DetachDevice(const Arguments& args);
            static Handle<Value> UpdateDevice(const Arguments& args);
            static Handle<Value> ToXml(const Arguments& args);
            static Handle<Value> GetJobInfo(const Arguments& args);
            static Handle<Value> AbortCurrentJob(const Arguments& args);
            static Handle<Value> GetSchedType(const Arguments& args);
            static Handle<Value> GetSchedParams(const Arguments& args);
            static Handle<Value> SetSchedParams(const Arguments& args);
            static Handle<Value> GetSecurityLabel(const Arguments& args);
            static Handle<Value> SaveManagedImage(const Arguments& args);
            static Handle<Value> RemoveManagedImage(const Arguments& args);
            static Handle<Value> HasManagedImage(const Arguments& args);
            static Handle<Value> MemoryPeek(const Arguments& args);
            static Handle<Value> GetMemoryStats(const Arguments& args);
            static Handle<Value> BlockPeek(const Arguments& args);
            static Handle<Value> GetBlockStats(const Arguments& args);
            static Handle<Value> GetBlockInfo(const Arguments& args);
            static Handle<Value> CoreDump(const Arguments& args);
            static Handle<Value> GetInterfaceStats(const Arguments& args);
            static Handle<Value> HasCurrentSnapshot(const Arguments& args);
            static Handle<Value> RevertToSnapshot(const Arguments& args);
            static Handle<Value> TakeSnapshot(const Arguments& args);
            static Handle<Value> GetCurrentSnapshot(const Arguments& args);
            static Handle<Value> DeleteSnapshot(const Arguments& args);
            static Handle<Value> LookupSnapshotByName(const Arguments& args);
            static Handle<Value> GetSnapshots(const Arguments& args);

        private:
            virDomainPtr domain_;
            static Persistent<FunctionTemplate> constructor_template;
    };

    class LookupDomainByNameWorker : public LibvirtWorker {
        public:
            LookupDomainByNameWorker(NanCallback *callback, Hypervisor *hypervisor, char *name)
                : LibvirtWorker(callback, hypervisor) { name_ = strdup(name); }
            ~LookupDomainByNameWorker() { free(name_); }
            void Execute();
        protected:
            void HandleOKCallback();
        private:
            char *name_;
            virDomainPtr domainptr_;
    };

}  //namespace NodeLibvirt

#endif  // SRC_DOMAIN_H
