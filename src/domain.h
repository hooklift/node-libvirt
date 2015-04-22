// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_DOMAIN_H_
#define SRC_DOMAIN_H_

#include "node_libvirt.h"

#include "worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class Domain : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(const LibVirtHandle &handle);
  virtual ~Domain();

private:
  explicit Domain(virDomainPtr handle) : handle_(handle) {}
  static Persistent<FunctionTemplate> constructor_template;
  static Persistent<Function> constructor;
  virDomainPtr handle_;

  friend class Hypervisor;

private:
  // HYPERVISOR METHODS
  static NAN_METHOD(LookupByName);
  static NAN_METHOD(LookupByUUID);
  static NAN_METHOD(LookupById);
  static NAN_METHOD(Create);
  static NAN_METHOD(Define);
  static NAN_METHOD(Restore);
  static NAN_METHOD(RegisterEvent);
  static NAN_METHOD(UnregisterEvent);

  // ACTIONS
  static NAN_METHOD(Destroy);
  static NAN_METHOD(Start);
  static NAN_METHOD(Reboot);
  static NAN_METHOD(Reset);
  static NAN_METHOD(Suspend);
  static NAN_METHOD(Resume);
  static NAN_METHOD(Shutdown);
  static NAN_METHOD(Save);
  static NAN_METHOD(ManagedSave);
  static NAN_METHOD(ManagedSaveRemove);
  static NAN_METHOD(GetJobInfo);
  static NAN_METHOD(AbortCurrentJob);
  static NAN_METHOD(CoreDump);
  static NAN_METHOD(AttachDevice);
  static NAN_METHOD(DetachDevice);
  static NAN_METHOD(UpdateDevice);
  static NAN_METHOD(SendKeys);
  static NAN_METHOD(Migrate);
  static NAN_METHOD(PinVcpu);
  static NAN_METHOD(MemoryPeek);
  static NAN_METHOD(BlockPeek);
  static NAN_METHOD(RevertToSnapshot);
  static NAN_METHOD(TakeSnapshot);
  static NAN_METHOD(DeleteSnapshot);
  static NAN_METHOD(LookupSnapshotByName);

  // ACCESSORS/MUTATORS
  static NAN_METHOD(GetName);
  static NAN_METHOD(GetInfo);
  static NAN_METHOD(GetId);
  static NAN_METHOD(GetOSType);
  static NAN_METHOD(GetUUID);
  static NAN_METHOD(GetAutostart);
  static NAN_METHOD(SetAutostart);
  static NAN_METHOD(GetMaxMemory);
  static NAN_METHOD(SetMaxMemory);
  static NAN_METHOD(SetMemory);
  static NAN_METHOD(GetMaxVcpus);
  static NAN_METHOD(IsActive);
  static NAN_METHOD(IsPersistent);
  static NAN_METHOD(IsUpdated);
  static NAN_METHOD(HasManagedSaveImage);
  static NAN_METHOD(GetBlockInfo);
  static NAN_METHOD(GetBlockStats);
  static NAN_METHOD(GetSchedulerType);
  static NAN_METHOD(GetSchedulerParameters);
  static NAN_METHOD(GetSecurityLabel);
  static NAN_METHOD(GetInterfaceStats);
  static NAN_METHOD(GetMemoryStats);
  static NAN_METHOD(GetVcpus);
  static NAN_METHOD(SetVcpus);
  static NAN_METHOD(ToXml);
  static NAN_METHOD(GetCurrentSnapshot);
  static NAN_METHOD(SetMigrationMaxDowntime);
  static NAN_METHOD(HasCurrentSnapshot);
  static NAN_METHOD(GetSnapshots);

  // UNFINISHED SYNC ACCESSORS/MUTATORS
  static NAN_METHOD(SetSchedulerParameters);

private:
  // HYPERVISOR METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(Domain, LookupByName);
  NLV_LOOKUP_BY_VALUE_WORKER(Domain, LookupByUUID);
  NLV_LOOKUP_BY_VALUE_WORKER(Domain, Create);
  NLV_LOOKUP_BY_VALUE_WORKER(Domain, Define);

  class LookupByIdWorker : public LookupInstanceByValueWorker<Domain> {
  public:
    LookupByIdWorker(NanCallback *callback, const LibVirtHandle &handle, int id)
      : LookupInstanceByValueWorker<Domain>(callback, handle, std::string()), id_(id) {}
    void Execute();
  private:
    int id_;
  };

  class RestoreWorker : public PrimitiveReturnWorker<bool> {
  public:
    RestoreWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &path)
      : PrimitiveReturnWorker<bool>(callback, handle), path_(path) {}
    void Execute();
  private:
    std::string path_;
  };

  class CoreDumpWorker : public PrimitiveReturnWorker<bool> {
  public:
    CoreDumpWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &path)
      : PrimitiveReturnWorker<bool>(callback, handle), path_(path) {}
    void Execute();
  private:
    std::string path_;
  };

  class RegisterEventWorker : public PrimitiveReturnWorker<int> {
  public:
    RegisterEventWorker(NanCallback *callback, const LibVirtHandle &handle, Domain *domain, int eventId)
      : PrimitiveReturnWorker(callback, handle), domain_(domain), eventId_(eventId) {}
    void Execute();
  private:
    Domain *domain_;
    int eventId_;
  };

  class UnregisterEventWorker : public PrimitiveReturnWorker<bool> {
  public:
    UnregisterEventWorker(NanCallback *callback, const LibVirtHandle &handle, int callbackId)
      : PrimitiveReturnWorker(callback, handle), callbackId_(callbackId) {}
    void Execute();
  private:
    int callbackId_;
  };

  // ACTION METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Destroy, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Start, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Reboot, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Reset, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Suspend, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Resume, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Shutdown, bool);
  NLV_PRIMITIVE_RETURN_WORKER(AbortCurrentJob, bool);
  NLV_PRIMITIVE_RETURN_WORKER(ManagedSave, bool);
  NLV_PRIMITIVE_RETURN_WORKER(ManagedSaveRemove, bool);

  class SaveWorker : public PrimitiveReturnWorker<bool> {
  public:
    SaveWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &path)
      : PrimitiveReturnWorker<bool>(callback, handle), path_(path) {}
    void Execute();
  private:
    std::string path_;
  };

  class AttachDeviceWorker : public PrimitiveReturnWorker<bool> {
  public:
    AttachDeviceWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &xml, unsigned long flags)
      : PrimitiveReturnWorker<bool>(callback, handle), xml_(xml), flags_(flags) {}
    void Execute();
  private:
    std::string xml_;
    unsigned long flags_;
  };

  class DetachDeviceWorker : public PrimitiveReturnWorker<bool> {
  public:
    DetachDeviceWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &xml, unsigned long flags)
      : PrimitiveReturnWorker<bool>(callback, handle), xml_(xml), flags_(flags) {}
    void Execute();
  private:
    std::string xml_;
    unsigned long flags_;
  };

  class UpdateDeviceWorker : public PrimitiveReturnWorker<bool> {
  public:
    UpdateDeviceWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &xml, unsigned long flags)
      : PrimitiveReturnWorker<bool>(callback, handle), xml_(xml), flags_(flags) {}
    void Execute();
  private:
    std::string xml_;
    unsigned long flags_;
  };


  class SendKeysWorker : public PrimitiveReturnWorker<bool> {
  public:
    SendKeysWorker(NanCallback *callback, const LibVirtHandle &handle, const std::vector<unsigned int> &keys)
      : PrimitiveReturnWorker<bool>(callback, handle), keys_(keys) {}
    void Execute();
  private:
    std::vector<unsigned int> keys_;
  };

  class MigrateWorker : public LibVirtWorker {
  public:
    MigrateWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &uri)
      : LibVirtWorker(callback, handle), uri_(uri), conn_(NULL), migrated_(NULL), flags_(0), bandwidth_(0) {}
    MigrateWorker(NanCallback *callback, const LibVirtHandle &handle, virConnectPtr conn)
      : LibVirtWorker(callback, handle), conn_(conn), migrated_(NULL), flags_(0), bandwidth_(0) {}
    void Execute();
    void setFlags(const unsigned long flags) { flags_ = flags; }
    void setBandwidth(const unsigned long bandwidth) { bandwidth_ = bandwidth; }
    void setDestname(const std::string &destname) { destname_ = destname; }
  protected:
    virtual void HandleOKCallback();
  private:
    std::string uri_;
    virConnectPtr conn_;
    virDomainPtr migrated_;
    std::string destname_;
    unsigned long flags_;
    unsigned long bandwidth_;
  };

  class PinVcpuWorker : public PrimitiveReturnWorker<bool> {
  public:
    PinVcpuWorker(NanCallback *callback, const LibVirtHandle &handle, int vcpu, const std::vector<bool> &usables, const std::vector<int> &vcpus)
      : PrimitiveReturnWorker<bool>(callback, handle), vcpu_(vcpu), usables_(usables), vcpus_(vcpus) {}
    void Execute();
  private:
    int vcpu_;
    std::vector<bool> usables_;
    std::vector<int> vcpus_;
  };

  class MemoryPeekWorker : public LibVirtWorker {
  public:
    MemoryPeekWorker(NanCallback *callback, const LibVirtHandle &handle, unsigned long long start, size_t size, unsigned int flags)
      : LibVirtWorker(callback, handle), start_(start), size_(size), flags_(flags), buffer_(size) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    unsigned long long start_;
    size_t size_;
    unsigned int flags_;
    std::vector<char> buffer_;
  };

  class BlockPeekWorker : public LibVirtWorker {
  public:
    BlockPeekWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &path, unsigned long long start, size_t size, unsigned int flags)
      : LibVirtWorker(callback, handle), path_(path), start_(start), size_(size), flags_(flags), buffer_(size) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    std::string path_;
    unsigned long long start_;
    size_t size_;
    unsigned int flags_;
    std::vector<char> buffer_;
  };

  class RevertToSnapshotWorker : public LibVirtWorker {
  public:
    RevertToSnapshotWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &name)
      : LibVirtWorker(callback, handle), name_(name) {}
    void Execute();
  private:
    std::string name_;
  };

  class TakeSnapshotWorker : public LibVirtWorker {
  public:
    TakeSnapshotWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &xml, unsigned int flags)
      : LibVirtWorker(callback, handle), xml_(xml), flags_(flags) {}
    void Execute();
  private:
    std::string xml_;
    unsigned int flags_;
  };

  class DeleteSnapshotWorker : public LibVirtWorker {
  public:
    DeleteSnapshotWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &name)
      : LibVirtWorker(callback, handle), name_(name) {}
    void Execute();
  private:
    std::string name_;
  };

  class LookupSnapshotByNameWorker : public PrimitiveReturnWorker<std::string> {
  public:
    LookupSnapshotByNameWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &name)
      : PrimitiveReturnWorker(callback, handle), name_(name) {}
    void Execute();
  private:
    std::string name_;
  };

  // ACCESSOR/MUTATOR METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(GetName, std::string);
  NLV_OBJECT_RETURN_WORKER(GetInfo, virDomainInfo);
  NLV_PRIMITIVE_RETURN_WORKER(GetId, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetOSType, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetUUID, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetAutostart, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetMaxMemory, double);
  NLV_PRIMITIVE_RETURN_WORKER(GetMaxVcpus, int);
  NLV_PRIMITIVE_RETURN_WORKER(IsActive, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsPersistent, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsUpdated, bool);
  NLV_PRIMITIVE_RETURN_WORKER(HasManagedSaveImage, bool);
  NLV_PRIMITIVE_RETURN_WORKER(ToXml, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetSchedulerType, std::string);
  NLV_TYPED_PARAMETER_RETURN_WORKER(GetSchedulerParameters, virSchedParameter);
  NLV_OBJECT_RETURN_WORKER(GetSecurityLabel, virSecurityLabel);
  NLV_OBJECT_RETURN_WORKER(GetJobInfo, virDomainJobInfo);
  NLV_PRIMITIVE_RETURN_WORKER(GetCurrentSnapshot, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(HasCurrentSnapshot, bool);

  class SetAutostartWorker : public PrimitiveReturnWorker<bool> {
  public:
    SetAutostartWorker(NanCallback *callback, const LibVirtHandle &handle, bool autoStart)
      : PrimitiveReturnWorker<bool>(callback, handle), autoStart_(autoStart) {}
    void Execute();
  private:
    bool autoStart_;
  };

  class SetMaxMemoryWorker : public PrimitiveReturnWorker<bool> {
  public:
    SetMaxMemoryWorker(NanCallback *callback, const LibVirtHandle &handle, unsigned long maxMemory)
      : PrimitiveReturnWorker<bool>(callback, handle), maxMemory_(maxMemory) {}
    void Execute();
  private:
    unsigned long maxMemory_;
  };

  class SetMemoryWorker : public PrimitiveReturnWorker<bool> {
  public:
    SetMemoryWorker(NanCallback *callback, const LibVirtHandle &handle, unsigned long memory)
      : PrimitiveReturnWorker<bool>(callback, handle), memory_(memory) {}
    void Execute();
  private:
    unsigned long memory_;
  };

  class GetBlockInfoWorker : public LibVirtWorker {
  public:
    GetBlockInfoWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &path)
      : LibVirtWorker(callback, handle), path_(path) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    std::string path_;
    virDomainBlockInfo info_;
  };

  class GetBlockStatsWorker : public LibVirtWorker {
  public:
    GetBlockStatsWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &path)
      : LibVirtWorker(callback, handle), path_(path) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    std::string path_;
    virDomainBlockStatsStruct stats_;
  };

  class GetInterfaceStatsWorker : public LibVirtWorker {
  public:
    GetInterfaceStatsWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &interface)
      : LibVirtWorker(callback, handle), interface_(interface) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    std::string interface_;
    virDomainInterfaceStatsStruct stats_;
  };

  class GetMemoryStatsWorker : public LibVirtWorker
  {
  public:
    explicit GetMemoryStatsWorker(NanCallback *callback, const LibVirtHandle &handle)
      : LibVirtWorker(callback, handle) {}
    void Execute();
  protected:
    virtual void HandleOKCallback();
  private:
    std::vector<virDomainMemoryStatStruct> stats_;
  };

  class GetVcpusWorker : public LibVirtWorker
  {
  public:
    explicit GetVcpusWorker(NanCallback *callback, const LibVirtHandle &handle)
      : LibVirtWorker(callback, handle) {}
    void Execute();
  protected:
    virtual void HandleOKCallback();
  private:
    virNodeInfo nodeInfo_;
    std::vector<virVcpuInfo> info_;
    std::vector<unsigned char> map_;
  };

  class SetVcpusWorker : public PrimitiveReturnWorker<bool> {
  public:
    SetVcpusWorker(NanCallback *callback, const LibVirtHandle &handle, unsigned int count)
      : PrimitiveReturnWorker<bool>(callback, handle), count_(count) {}
    void Execute();
  private:
    unsigned int count_;
  };

  class SetMigrationMaxDowntimeWorker : public PrimitiveReturnWorker<bool> {
  public:
    SetMigrationMaxDowntimeWorker(NanCallback *callback, const LibVirtHandle &handle, long long downtime, unsigned int flags)
      : PrimitiveReturnWorker(callback, handle), downtime_(downtime), flags_(flags) {}
    void Execute();
  private:
    long long downtime_;
    unsigned int flags_;
  };

  class GetSnapshotsWorker : public LibVirtWorker {
  public:
    GetSnapshotsWorker(NanCallback *callback, const LibVirtHandle &handle)
      : LibVirtWorker(callback, handle) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    std::vector<std::string> xmls_;
  };

private:
  static void domain_event_free(void *opaque);
  static int domain_event_lifecycle_callback(virConnectPtr conn, virDomainPtr domain,
                                             int event, int detail, void *opaque);
  static int domain_event_generic_callback(virConnectPtr conn, virDomainPtr domain, void *opaque);
  static int domain_event_rtcchange_callback(virConnectPtr conn, virDomainPtr domain, long long utcoffset, void *opaque);
  static int domain_event_watchdog_callback(virConnectPtr conn, virDomainPtr domain, int action, void *opaque);
  static int domain_event_io_error_callback(virConnectPtr conn, virDomainPtr domain, const char *src_path,
                                            const char *dev_alias, int action, void *opaque);
  static int domain_event_io_error_reason_callback(virConnectPtr conn, virDomainPtr domain, const char *src_path,
                                                   const char *dev_alias, int action, const char *reason, void *opaque);
  static int domain_event_graphics_callback(virConnectPtr conn, virDomainPtr domain, int phase,
                                            virDomainEventGraphicsAddressPtr local,
                                            virDomainEventGraphicsAddressPtr remote,
                                            const char *authScheme,
                                            virDomainEventGraphicsSubjectPtr subject,
                                            void *opaque);
};

}  //namespace NodeLibvirt

#endif  // SRC_DOMAIN_H
