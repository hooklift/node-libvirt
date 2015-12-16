// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef DOMAIN_H
#define DOMAIN_H

#include "nlv_object.h"
#include "nlv_async_worker.h"
#include "worker_macros.h"

#include "hypervisor.h"

#if LIBVIR_CHECK_VERSION(0,9,10)
#define _HAVE_DOMAIN_METADATA_API 1
#endif

namespace NLV {

struct DomainCleanupHandler {
  static int cleanup(virDomainPtr handle) {
    return virDomainFree(handle);
  }
};

class Domain : public NLVObject<virDomainPtr, DomainCleanupHandler>
{
public:
  static void Initialize(Handle<Object> exports);
  static Local<Object> NewInstance(virDomainPtr handle);

private:
  explicit Domain(virDomainPtr handle);
  static Nan::Persistent<FunctionTemplate> constructor_template;
  static Nan::Persistent<Function> constructor;
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
  static NAN_METHOD(Undefine);

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
  static NAN_METHOD(GetMetadata);
  static NAN_METHOD(SetMetadata);
  static NAN_METHOD(GetCurrentSnapshot);
  static NAN_METHOD(SetMigrationMaxDowntime);
  static NAN_METHOD(HasCurrentSnapshot);
  static NAN_METHOD(GetSnapshots);

  // UNFINISHED SYNC ACCESSORS/MUTATORS
  static NAN_METHOD(SetSchedulerParameters);

private:
  // HYPERVISOR METHOD WORKERS
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByName, Domain, Hypervisor, virDomainPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(LookupByUUID, Domain, Hypervisor, virDomainPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Create, Domain, Hypervisor, virDomainPtr);
  NLV_LOOKUP_BY_VALUE_WORKER(Define, Domain, Hypervisor, virDomainPtr);

  class LookupByIdWorker : public NLVLookupInstanceByValueWorker<Domain, Hypervisor, virDomainPtr> {
  public:
    LookupByIdWorker(Nan::Callback *callback, Hypervisor *parent, int id)
      : NLVLookupInstanceByValueWorker<Domain, Hypervisor, virDomainPtr>(callback, parent, std::string()), id_(id) {}
    void Execute();
  private:
    int id_;
  };

  class RestoreWorker : public NLVPrimitiveReturnWorker<virConnectPtr, bool> {
  public:
    RestoreWorker(Nan::Callback *callback, virConnectPtr handle, const std::string &path)
      : NLVPrimitiveReturnWorker<virConnectPtr, bool>(callback, handle), path_(path) {}
    void Execute();
  private:
    std::string path_;
  };

  class CoreDumpWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    CoreDumpWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &path)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), path_(path) {}
    void Execute();
  private:
    std::string path_;
  };

  class RegisterEventWorker : public NLVPrimitiveReturnWorker<virDomainPtr, int> {
  public:
    RegisterEventWorker(Nan::Callback *callback, virDomainPtr handle, Domain *domain, int eventId)
      : NLVPrimitiveReturnWorker<virDomainPtr, int>(callback, handle), domain_(domain), eventId_(eventId) {}
    void Execute();
  private:
    Domain *domain_;
    int eventId_;
  };

  class UnregisterEventWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    UnregisterEventWorker(Nan::Callback *callback, virDomainPtr handle, int callbackId)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), callbackId_(callbackId) {}
    void Execute();
  private:
    int callbackId_;
  };

  // ACTION METHOD WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(Destroy, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Start, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Reboot, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Reset, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Suspend, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Resume, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Shutdown, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(AbortCurrentJob, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(ManagedSave, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(ManagedSaveRemove, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(Undefine, virDomainPtr, bool);

  class SaveWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    SaveWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &path)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), path_(path) {}
    void Execute();
  private:
    std::string path_;
  };

  class AttachDeviceWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    AttachDeviceWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &xml, unsigned long flags)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), xml_(xml), flags_(flags) {}
    void Execute();
  private:
    std::string xml_;
    unsigned long flags_;
  };

  class DetachDeviceWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    DetachDeviceWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &xml, unsigned long flags)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), xml_(xml), flags_(flags) {}
    void Execute();
  private:
    std::string xml_;
    unsigned long flags_;
  };

  class UpdateDeviceWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    UpdateDeviceWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &xml, unsigned long flags)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), xml_(xml), flags_(flags) {}
    void Execute();
  private:
    std::string xml_;
    unsigned long flags_;
  };


  class SendKeysWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    SendKeysWorker(Nan::Callback *callback, virDomainPtr handle, const std::vector<unsigned int> &keys)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), keys_(keys) {}
    void Execute();
  private:
    std::vector<unsigned int> keys_;
  };

  class MigrateWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    MigrateWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &uri)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), uri_(uri), conn_(NULL), migrated_(NULL), flags_(0), bandwidth_(0) {}
    MigrateWorker(Nan::Callback *callback, virDomainPtr handle, virConnectPtr conn)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), conn_(conn), migrated_(NULL), flags_(0), bandwidth_(0) {}
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

  class PinVcpuWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    PinVcpuWorker(Nan::Callback *callback, virDomainPtr handle, int vcpu, const std::vector<bool> &usables, const std::vector<int> &vcpus)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), vcpu_(vcpu), usables_(usables), vcpus_(vcpus) {}
    void Execute();
  private:
    int vcpu_;
    std::vector<bool> usables_;
    std::vector<int> vcpus_;
  };

  class MemoryPeekWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    MemoryPeekWorker(Nan::Callback *callback, virDomainPtr handle, unsigned long long start, size_t size, unsigned int flags)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), start_(start), size_(size), flags_(flags), buffer_(size) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    unsigned long long start_;
    size_t size_;
    unsigned int flags_;
    std::vector<char> buffer_;
  };

  class BlockPeekWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    BlockPeekWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &path, unsigned long long start, size_t size, unsigned int flags)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), path_(path), start_(start), size_(size), flags_(flags), buffer_(size) {}
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

  class RevertToSnapshotWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    RevertToSnapshotWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &name)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), name_(name) {}
    void Execute();
  private:
    std::string name_;
  };

  class TakeSnapshotWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    TakeSnapshotWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &xml, unsigned int flags)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), xml_(xml), flags_(flags) {}
    void Execute();
  private:
    std::string xml_;
    unsigned int flags_;
  };

  class DeleteSnapshotWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    DeleteSnapshotWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &name)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), name_(name) {}
    void Execute();
  private:
    std::string name_;
  };

  class LookupSnapshotByNameWorker : public NLVStringReturnWorker<virDomainPtr, std::string> {
  public:
    LookupSnapshotByNameWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &name)
      : NLVStringReturnWorker<virDomainPtr, std::string>(callback, handle), name_(name) {}
    void Execute();
  private:
    std::string name_;
  };

  // ACCESSOR/MUTATOR METHOD WORKERS
  NLV_STRING_RETURN_WORKER(GetName, virDomainPtr, std::string);
  NLV_OBJECT_RETURN_WORKER(GetInfo, virDomainPtr, virDomainInfo);
  NLV_PRIMITIVE_RETURN_WORKER(GetId, virDomainPtr, int);
  NLV_STRING_RETURN_WORKER(GetOSType, virDomainPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetUUID, virDomainPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetAutostart, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetMaxMemory, virDomainPtr, double);
  NLV_PRIMITIVE_RETURN_WORKER(GetMaxVcpus, virDomainPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(IsActive, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsPersistent, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsUpdated, virDomainPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(HasManagedSaveImage, virDomainPtr, bool);
  NLV_STRING_RETURN_WORKER(GetSchedulerType, virDomainPtr, std::string);
  NLV_TYPED_PARAMETER_RETURN_WORKER(GetSchedulerParameters, virDomainPtr, virSchedParameter);
  NLV_OBJECT_RETURN_WORKER(GetSecurityLabel, virDomainPtr, virSecurityLabel);
  NLV_OBJECT_RETURN_WORKER(GetJobInfo, virDomainPtr, virDomainJobInfo);
  NLV_STRING_RETURN_WORKER(GetCurrentSnapshot, virDomainPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(HasCurrentSnapshot, virDomainPtr, bool);

  class ToXmlWorker : public NLVStringReturnWorker<virDomainPtr, std::string> {
    public:
      ToXmlWorker(Nan::Callback *callback, virDomainPtr handle, int flags = 0)
        : NLVStringReturnWorker<virDomainPtr, std::string>(callback, handle), flags_(flags) {}
      void Execute();
    private:
      unsigned int flags_;
  };

#ifdef _HAVE_DOMAIN_METADATA_API
  class GetMetadataWorker : public NLVStringReturnWorker<virDomainPtr, std::string> {
    public:
      GetMetadataWorker(Nan::Callback *callback, virDomainPtr handle, int type, const std::string &namespace_uri, unsigned int flags)
        : NLVStringReturnWorker<virDomainPtr, std::string>(callback, handle), type_(type), namespace_uri_(namespace_uri), flags_(flags) {}
      void Execute();
    private:
      int type_;
      std::string namespace_uri_;
      unsigned int flags_;
  };

  class SetMetadataWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
    public:
      SetMetadataWorker(Nan::Callback *callback, virDomainPtr handle,
	      int type, bool null_metadata,
	      const std::string &metadata, const std::string &namespace_key,
	      const std::string &namespace_uri, unsigned int flags)
        : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle),
	  type_(type), null_metadata_(null_metadata),
	  metadata_(metadata), namespace_key_(namespace_key),
	  namespace_uri_(namespace_uri), flags_(flags) {}
      void Execute();
    private:
      int type_;
      bool null_metadata_;
      std::string metadata_;
      std::string namespace_key_;
      std::string namespace_uri_;
      unsigned int flags_;
  };
#endif

  class SetAutostartWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    SetAutostartWorker(Nan::Callback *callback, virDomainPtr handle, bool autoStart)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), autoStart_(autoStart) {}
    void Execute();
  private:
    bool autoStart_;
  };

  class SetMaxMemoryWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    SetMaxMemoryWorker(Nan::Callback *callback, virDomainPtr handle, unsigned long maxMemory)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), maxMemory_(maxMemory) {}
    void Execute();
  private:
    unsigned long maxMemory_;
  };

  class SetMemoryWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    SetMemoryWorker(Nan::Callback *callback, virDomainPtr handle, unsigned long memory)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), memory_(memory) {}
    void Execute();
  private:
    unsigned long memory_;
  };

  class GetBlockInfoWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    GetBlockInfoWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &path)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), path_(path) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    std::string path_;
    virDomainBlockInfo info_;
  };

  class GetBlockStatsWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    GetBlockStatsWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &path)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), path_(path) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    std::string path_;
    virDomainBlockStatsStruct stats_;
  };

  class GetInterfaceStatsWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    GetInterfaceStatsWorker(Nan::Callback *callback, virDomainPtr handle, const std::string &interface)
      : NLVAsyncWorker<virDomainPtr>(callback, handle), interface_(interface) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    std::string interface_;
    virDomainInterfaceStatsStruct stats_;
  };

  class GetMemoryStatsWorker : public NLVAsyncWorker<virDomainPtr>
  {
  public:
    explicit GetMemoryStatsWorker(Nan::Callback *callback, virDomainPtr handle)
      : NLVAsyncWorker<virDomainPtr>(callback, handle) {}
    void Execute();
  protected:
    virtual void HandleOKCallback();
  private:
    std::vector<virDomainMemoryStatStruct> stats_;
  };

  class GetVcpusWorker : public NLVAsyncWorker<virDomainPtr>
  {
  public:
    explicit GetVcpusWorker(Nan::Callback *callback, virDomainPtr handle)
      : NLVAsyncWorker<virDomainPtr>(callback, handle) {}
    void Execute();
  protected:
    virtual void HandleOKCallback();
  private:
    virNodeInfo nodeInfo_;
    std::vector<virVcpuInfo> info_;
    std::vector<unsigned char> map_;
  };

  class SetVcpusWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    SetVcpusWorker(Nan::Callback *callback, virDomainPtr handle, unsigned int count)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), count_(count) {}
    void Execute();
  private:
    unsigned int count_;
  };

  class SetMigrationMaxDowntimeWorker : public NLVPrimitiveReturnWorker<virDomainPtr, bool> {
  public:
    SetMigrationMaxDowntimeWorker(Nan::Callback *callback, virDomainPtr handle, long long downtime, unsigned int flags)
      : NLVPrimitiveReturnWorker<virDomainPtr, bool>(callback, handle), downtime_(downtime), flags_(flags) {}
    void Execute();
  private:
    long long downtime_;
    unsigned int flags_;
  };

  class GetSnapshotsWorker : public NLVAsyncWorker<virDomainPtr> {
  public:
    GetSnapshotsWorker(Nan::Callback *callback, virDomainPtr handle)
      : NLVAsyncWorker<virDomainPtr>(callback, handle) {}
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

}  //namespace NLV

#endif  // DOMAIN_H
