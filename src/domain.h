// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_DOMAIN_H_
#define SRC_DOMAIN_H_

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {

    class Domain : public EventEmitter {
        friend class Hypervisor;

        public:
            static void Initialize();
            //static Handle<Value> New(Hypervisor hypervisor);
            static inline bool HasInstance(v8::Handle<v8::Value> value) {
                if (!value->IsObject()) {
                    return false;
                }
                v8::Local<v8::Object> object = value->ToObject();
                return constructor_template->HasInstance(object);
            }
            virtual ~Domain();

        protected:
            //static Handle<Value> New(const Arguments& args);
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
            static Handle<Value> Reboot(const Arguments& args);
            static Handle<Value> Save(const Arguments& args);
            static Handle<Value> Restore(const Arguments& args);
            static Handle<Value> Suspend(const Arguments& args);
            static Handle<Value> Resume(const Arguments& args);
            static Handle<Value> Shutdown(const Arguments& args);
            static Handle<Value> Start(const Arguments& args);
            static Handle<Value> GetVcpus(const Arguments& args);
            static Handle<Value> SetVcpus(const Arguments& args);
            static Handle<Value> Migrate(const Arguments& args);
            static Handle<Value> SetMigrationMaxDowntime(const Arguments& args);
            static Handle<Value> PinVcpu(const Arguments& args);
            static Handle<Value> AttachDevice(const Arguments& args);
            static Handle<Value> DetachDevice(const Arguments& args);
            static Handle<Value> UpdateDevice(const Arguments& args);
            static Handle<Value> Destroy(const Arguments& args);
            static Handle<Value> ToXml(const Arguments& args);


            static Handle<Value> GetJobInfo(const Arguments& args);
            static Handle<Value> GetSchedParams(const Arguments& args);
            static Handle<Value> SetSchedParams(const Arguments& args);
            static Handle<Value> GetSchedType(const Arguments& args);
            static Handle<Value> GetSecurityLabel(const Arguments& args);

            static Handle<Value> HasManagedSaveImage(const Arguments& args);
            static Handle<Value> GetInterfaceStats(const Arguments& args);
            static Handle<Value> ManagedSave(const Arguments& args);
            static Handle<Value> ManagedSaveRemove(const Arguments& args);
            static Handle<Value> MemoryPeek(const Arguments& args);
            static Handle<Value> GetMemoryStats(const Arguments& args);

            static Handle<Value> HasCurrentSnapshot(const Arguments& args);
            static Handle<Value> RevertToSnapshot(const Arguments& args);
            static Handle<Value> CreateSnapshotXml(const Arguments& args);
            static Handle<Value> GetCurrentSnapshot(const Arguments& args);
            static Handle<Value> DeleteSnapshot(const Arguments& args);
            static Handle<Value> FreeSnapshot(const Arguments& args);// maybe yes maybe not ?
            static Handle<Value> GetSnapshotXml(const Arguments& args);
            static Handle<Value> ListSnapshotsNames(const Arguments& args);
            static Handle<Value> LookupSnapshotByName(const Arguments& args);
            static Handle<Value> GetSnapshotsCount(const Arguments& args);




        private:
            virDomainPtr domain_;
            static Persistent<FunctionTemplate> constructor_template;
    };

}  //namespace NodeLibvirt

#endif  // SRC_DOMAIN_H

