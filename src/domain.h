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
        friend class DomainCreateWorker;
        friend class DomainDefineWorker;
        friend class DomainLookupByNameWorker;
        friend class DomainLookupByIdWorker;

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
            static NAN_METHOD(Create);
            static NAN_METHOD(Define);
            static NAN_METHOD(LookupById);
            static NAN_METHOD(LookupByName);
            static Handle<Value> LookupByUUID(const Arguments& args);
            static Handle<Value> Undefine(const Arguments& args);
            static NAN_METHOD(GetId);
            static NAN_METHOD(GetInfo);
            static NAN_METHOD(GetName);
            static NAN_METHOD(GetUUID);
            static NAN_METHOD(GetAutostart);
            static NAN_METHOD(SetAutostart);
            static NAN_METHOD(GetOsType);
            static NAN_METHOD(GetMaxMemory);
            static NAN_METHOD(SetMaxMemory);
            static NAN_METHOD(SetMemory);
            static NAN_METHOD(GetMaxVcpus);
            static NAN_METHOD(IsActive);
            static NAN_METHOD(IsPersistent);
            static NAN_METHOD(IsUpdated);
            static NAN_METHOD(Reboot);
            static NAN_METHOD(Reset);
            static NAN_METHOD(Save);
            static NAN_METHOD(Restore);
            static NAN_METHOD(Suspend);
            static NAN_METHOD(Resume);
            static NAN_METHOD(Shutdown);
            static NAN_METHOD(Start);
            static Handle<Value> Destroy(const Arguments& args);
            static Handle<Value> SendKey(const Arguments& args);
            static NAN_METHOD(GetVcpus);
            static NAN_METHOD(SetVcpus);
            static Handle<Value> Migrate(const Arguments& args);
            static Handle<Value> SetMigrationMaxDowntime(const Arguments& args);
            static NAN_METHOD(PinVcpu);
            static NAN_METHOD(AttachDevice);
            static NAN_METHOD(DetachDevice);
            static NAN_METHOD(UpdateDevice);
            static NAN_METHOD(ToXml);
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

    class DomainCreateWorker : public LibvirtWorker {
        public:
            DomainCreateWorker(NanCallback *callback, virConnectPtr conn, const char *xml, unsigned int flags = 0)
                : LibvirtWorker(callback, conn), xml_(xml), flags_(flags) {}
            void Execute();
        protected:
            void HandleOKCallback();
        private:
            std::string xml_;
            unsigned int flags_;
            virDomainPtr domainptr_;
    };

    class DomainDefineWorker : public LibvirtWorker {
        public:
            DomainDefineWorker(NanCallback *callback, virConnectPtr conn, const char *xml, unsigned int flags = 0)
                : LibvirtWorker(callback, conn), xml_(xml), flags_(flags) {}
            void Execute();
        protected:
            void HandleOKCallback();
        private:
            std::string xml_;
            unsigned int flags_;
            virDomainPtr domainptr_;
    };

    class DomainLookupByIdWorker : public LibvirtWorker {
        public:
            DomainLookupByIdWorker(NanCallback *callback, virConnectPtr conn, int id)
                : LibvirtWorker(callback, conn), id_(id) {}

            void Execute();
        protected:
            void HandleOKCallback();
        private:
            int id_;
            virDomainPtr domainptr_;
    };

    class DomainLookupByNameWorker : public LibvirtWorker {
        public:
            DomainLookupByNameWorker(NanCallback *callback, virConnectPtr conn, char *name)
                : LibvirtWorker(callback, conn), name_(name) {}

            void Execute();
        protected:
            void HandleOKCallback();
        private:
            std::string name_;
            virDomainPtr domainptr_;
    };

    class DomainWorker : public LibvirtWorker {
        public:
            DomainWorker(NanCallback *callback, virDomainPtr domainptr)
                : LibvirtWorker(callback, associateConn(domainptr)), domainptr_(domainptr) {
            }

            virDomainPtr getDomainPtr() { return domainptr_; }
        private:
            virConnectPtr associateConn(virDomainPtr domainptr) {
                return virDomainGetConnect(domainptr);
            }

            virDomainPtr domainptr_;
    };

    class DomainGetIdWorker : public HelperWorker<DomainWorker, virDomainPtr, int, Integer> {
        public:
            DomainGetIdWorker(NanCallback *callback, virDomainPtr domainptr)
                : HelperWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainGetInfoWorker : public DomainWorker {
        public:
            DomainGetInfoWorker(NanCallback *callback, virDomainPtr domainptr)
                : DomainWorker(callback, domainptr), info_(NULL) {
            }

            void Execute();
        protected:
            void HandleOKCallback();
        private:
            virDomainInfoPtr info_;
    };

    class DomainGetNameWorker : public StringReturnWorker<DomainWorker, virDomainPtr> {
        public:
            DomainGetNameWorker(NanCallback *callback, virDomainPtr domainptr)
                : StringReturnWorker(callback, domainptr, false) {}
            void Execute();
    };

    class DomainGetUUIDWorker : public StringReturnWorker<DomainWorker, virDomainPtr> {
        public:
            DomainGetUUIDWorker(NanCallback *callback, virDomainPtr domainptr)
                : StringReturnWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainGetAutostartWorker : public BooleanReturnWorker<DomainWorker, virDomainPtr> {
        public:
            DomainGetAutostartWorker(NanCallback *callback, virDomainPtr domainptr)
                : BooleanReturnWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainSetAutostartWorker : public BooleanReturnWorker<DomainWorker, virDomainPtr> {
        public:
            DomainSetAutostartWorker(NanCallback *callback, virDomainPtr domainptr, int autostart)
                : BooleanReturnWorker(callback, domainptr), autostart_(autostart) {}
            void Execute();
        private:
            int autostart_;
    };

    class DomainGetOsTypeWorker : public StringReturnWorker<DomainWorker, virDomainPtr> {
        public:
            DomainGetOsTypeWorker(NanCallback *callback, virDomainPtr domainptr)
                : StringReturnWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainGetMaxMemoryWorker : public HelperWorker<DomainWorker, virDomainPtr, unsigned long, Number> {
        public:
            DomainGetMaxMemoryWorker(NanCallback *callback, virDomainPtr domainptr)
                : HelperWorker(callback, domainptr, 0L) {}
            void Execute();
    };

    class DomainSetMaxMemoryWorker : public DomainWorker {
        public:
            DomainSetMaxMemoryWorker(NanCallback *callback, virDomainPtr domainptr, unsigned long maxmem)
                : DomainWorker(callback, domainptr), maxmem_(maxmem) {}
            void Execute();
        private:
            unsigned long maxmem_;
    };

    class DomainSetMemoryWorker : public DomainWorker {
        public:
            DomainSetMemoryWorker(NanCallback *callback, virDomainPtr domainptr, unsigned long mem)
                : DomainWorker(callback, domainptr), mem_(mem) {}
            void Execute();
        private:
            unsigned long mem_;
    };

    class DomainGetMaxVcpusWorker : public HelperWorker<DomainWorker, virDomainPtr, int, Integer> {
        public:
            DomainGetMaxVcpusWorker(NanCallback *callback, virDomainPtr domainptr)
                : HelperWorker(callback, domainptr, -1) {}
            void Execute();
    };

    class DomainIsActiveWorker : public BooleanReturnWorker<DomainWorker, virDomainPtr> {
        public:
            DomainIsActiveWorker(NanCallback *callback, virDomainPtr domainptr)
                : BooleanReturnWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainIsPersistentWorker : public BooleanReturnWorker<DomainWorker, virDomainPtr> {
        public:
            DomainIsPersistentWorker(NanCallback *callback, virDomainPtr domainptr)
                : BooleanReturnWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainIsUpdatedWorker : public BooleanReturnWorker<DomainWorker, virDomainPtr> {
        public:
            DomainIsUpdatedWorker(NanCallback *callback, virDomainPtr domainptr)
                : BooleanReturnWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainRebootWorker : public DomainWorker {
        public:
            DomainRebootWorker(NanCallback *callback, virDomainPtr domainptr)
                : DomainWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainResetWorker : public DomainWorker {
        public:
            DomainResetWorker(NanCallback *callback, virDomainPtr domainptr)
                : DomainWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainSaveWorker : public DomainWorker {
        public:
            DomainSaveWorker(NanCallback *callback, virDomainPtr domainptr, char *path)
                : DomainWorker(callback, domainptr), path_(path) {}
            void Execute();
        private:
            std::string path_;
    };

    class DomainRestoreWorker : public LibvirtWorker {
        public:
            DomainRestoreWorker(NanCallback *callback, virConnectPtr conn, char *path)
                : LibvirtWorker(callback, conn), path_(path) {}
            void Execute();
        private:
            std::string path_;
    };

    class DomainSuspendWorker : public DomainWorker {
        public:
            DomainSuspendWorker(NanCallback *callback, virDomainPtr domainptr)
                : DomainWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainResumeWorker : public DomainWorker {
        public:
            DomainResumeWorker(NanCallback *callback, virDomainPtr domainptr)
                : DomainWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainShutdownWorker : public DomainWorker {
        public:
            DomainShutdownWorker(NanCallback *callback, virDomainPtr domainptr)
                : DomainWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainStartWorker : public DomainWorker {
        public:
            DomainStartWorker(NanCallback *callback, virDomainPtr domainptr)
                : DomainWorker(callback, domainptr) {}
            void Execute();
    };

    class DomainGetVcpusWorker : public DomainWorker {
        public:
            DomainGetVcpusWorker(NanCallback *callback, virDomainPtr domainptr)
                : DomainWorker(callback, domainptr) {
            }

            void Execute();
        protected:
            void HandleOKCallback();
        private:
            virDomainInfo info_;
            virNodeInfo nodeinfo_;
            int cpumaplen_;
            std::vector<virVcpuInfo> cpuinfo_;
            std::vector<unsigned char> cpumap_;
    };

    class DomainSetVcpusWorker : public DomainWorker {
        public:
            DomainSetVcpusWorker(NanCallback *callback, virDomainPtr domainptr, unsigned int vcpus)
                : DomainWorker(callback, domainptr), vcpus_(vcpus) {}
            void Execute();
        private:
            unsigned int vcpus_;
    };

    class DomainPinVcpuWorker : public DomainWorker {
        public:
            DomainPinVcpuWorker(NanCallback *callback, virDomainPtr domainptr, int vcpu, Handle<Array> cpus);
            void Execute();
        private:
            int vcpu_;
            int length_;
            std::vector<bool> usables_;
            std::vector<int> vcpus_;
    };

    class DomainAttachDeviceWorker : public DomainWorker {
        public:
            DomainAttachDeviceWorker(NanCallback *callback, virDomainPtr domainptr, char *xml, unsigned int flags = 0)
                : DomainWorker(callback, domainptr), xml_(xml), flags_(flags) {}
            void Execute();
        private:
            std::string xml_;
            unsigned int flags_;
    };

    class DomainDetachDeviceWorker : public DomainWorker {
        public:
            DomainDetachDeviceWorker(NanCallback *callback, virDomainPtr domainptr, char *xml, unsigned int flags = 0)
                : DomainWorker(callback, domainptr), xml_(xml), flags_(flags) {}
            void Execute();
        private:
            std::string xml_;
            unsigned int flags_;
    };

    class DomainUpdateDeviceWorker : public DomainWorker {
        public:
            DomainUpdateDeviceWorker(NanCallback *callback, virDomainPtr domainptr, char *xml, unsigned int flags = 0)
                : DomainWorker(callback, domainptr), xml_(xml), flags_(flags) {}
            void Execute();
        private:
            std::string xml_;
            unsigned int flags_;
    };

    class DomainToXmlWorker : public StringReturnWorker<DomainWorker, virDomainPtr> {
        public:
            DomainToXmlWorker(NanCallback *callback, virDomainPtr domainptr, int flags)
                : StringReturnWorker(callback, domainptr), flags_(flags) {}
            void Execute();
        private:
            int flags_;
    };

}  //namespace NodeLibvirt

#endif  // SRC_DOMAIN_H
