// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_HYPERVISOR_H_
#define SRC_HYPERVISOR_H_

#include <nan.h>

#include "node_libvirt.h"
#include "worker.h"

using namespace v8;
using namespace node;
using namespace std;

#define HYPERVISOR_LIBVIRTWORKER(WorkerName) \
  class WorkerName : public LibVirtWorker { \
  public: \
    WorkerName(NanCallback *callback, Hypervisor *hypervisor) \
      : LibVirtWorker(callback, hypervisor->conn_), hypervisor_(hypervisor) {} \
    void Execute(); \
  private: \
    Hypervisor *hypervisor_; \
  };

#define HYPERVISOR_PRIMITIVE_RETURN_WORKER(WorkerName, Type)  \
  class WorkerName##Worker : public PrimitiveReturnWorker<Type> { \
  public: \
    WorkerName##Worker(NanCallback *callback, Hypervisor *hypervisor) \
      : PrimitiveReturnWorker<Type>(callback, hypervisor->conn_) {} \
    void Execute(); \
  };

#define HYPERVISOR_LIST_RETURN_WORKER(WorkerName, CType, V8Type)  \
  class WorkerName##Worker : public ListReturnWorker<CType, V8Type> { \
  public: \
    WorkerName##Worker(NanCallback *callback, Hypervisor *hypervisor) \
      : ListReturnWorker(callback, hypervisor->conn_) {}  \
    void Execute(); \
  };

#define HYPERVISOR_STRING_LIST_RETURN_WORKER(WorkerName)  \
  HYPERVISOR_LIST_RETURN_WORKER(WorkerName, std::string, v8::String)

#define HYPERVISOR_INT_LIST_RETURN_WORKER(WorkerName)  \
  HYPERVISOR_LIST_RETURN_WORKER(WorkerName, int, v8::Integer)

#define HYPERVISOR_DOUBLE_LIST_RETURN_WORKER(WorkerName)  \
  HYPERVISOR_DOUBLE_LIST_RETURN_WORKER(WorkerName, double, v8::Number)

namespace NodeLibvirt {

class Hypervisor : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);

  virConnectPtr Connection() const;

private:
  explicit Hypervisor(string uri, string user, string pass, bool readOnly);
  ~Hypervisor();

  static Persistent<Function> constructor;
  static Persistent<FunctionTemplate> constructor_template;

  virConnectPtr conn_;
  string uri_;
  string username_;
  string password_;
  bool readOnly_;

  friend class Interface;
  friend class Network;
  friend class NetworkFilter;
  friend class NodeDevice;
  friend class Secret;
  friend class StorageVolume;
  friend class StoragePool;

private:
  static NAN_METHOD(New);
  static NAN_METHOD(Connect);

  static NAN_METHOD(Disconnect);
  HYPERVISOR_LIBVIRTWORKER(DisconnectWorker)

  static NAN_METHOD(GetCapabilities);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetCapabilities, std::string)

  static NAN_METHOD(GetHostname);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetHostname, std::string)

  static NAN_METHOD(GetSysInfo);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetSysInfo, std::string)

  static NAN_METHOD(GetType);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetType, std::string)

  static NAN_METHOD(GetConnectionUri);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetConnectionUri, std::string)

  static NAN_METHOD(GetVersion);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetVersion, std::string)

  static NAN_METHOD(GetLibVirtVersion);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetLibVirtVersion, std::string)

  static NAN_METHOD(IsConnectionEncrypted);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(IsConnectionEncrypted, bool)

  static NAN_METHOD(IsConnectionSecure);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(IsConnectionSecure, bool)

  static NAN_METHOD(IsConnectionAlive);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(IsConnectionAlive, bool)

  static NAN_METHOD(GetMaxVcpus);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetMaxVcpusWorker, int)

  static NAN_METHOD(SetKeepAlive);
  static NAN_METHOD(GetBaselineCPU);
  static NAN_METHOD(CompareCPU);

  // virConnectList functions
  static NAN_METHOD(ListDefinedDomains);
  HYPERVISOR_STRING_LIST_RETURN_WORKER(ListDefinedDomains);

  static NAN_METHOD(ListDefinedNetworks);
  HYPERVISOR_STRING_LIST_RETURN_WORKER(ListDefinedNetworks);

  static NAN_METHOD(ListDefinedStoragePools);
  HYPERVISOR_STRING_LIST_RETURN_WORKER(ListDefinedStoragePools);

  static NAN_METHOD(ListDefinedInterfaces);
  HYPERVISOR_STRING_LIST_RETURN_WORKER(ListDefinedInterfaces);

  static NAN_METHOD(ListActiveDomains);
  HYPERVISOR_INT_LIST_RETURN_WORKER(ListActiveDomains);

  static NAN_METHOD(ListActiveInterfaces);
  HYPERVISOR_STRING_LIST_RETURN_WORKER(ListActiveInterfaces);

  static NAN_METHOD(ListNetworkFilters);
  HYPERVISOR_STRING_LIST_RETURN_WORKER(ListNetworkFilters);

  static NAN_METHOD(ListActiveNetworks);
  HYPERVISOR_STRING_LIST_RETURN_WORKER(ListActiveNetworks);

  static NAN_METHOD(ListSecrets);
  HYPERVISOR_STRING_LIST_RETURN_WORKER(ListSecrets);

  static NAN_METHOD(ListActiveStoragePools);
  HYPERVISOR_STRING_LIST_RETURN_WORKER(ListActiveStoragePools);


  // virConnectNumOf functions
  static NAN_METHOD(GetNumberOfDefinedDomains);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfDefinedDomains, int);

  static NAN_METHOD(GetNumberOfDefinedInterfaces);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfDefinedInterfaces, int);

  static NAN_METHOD(GetNumberOfDefinedNetworks);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfDefinedNetworks, int);

  static NAN_METHOD(GetNumberOfDefinedStoragePools);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfDefinedStoragePools, int);

  static NAN_METHOD(GetNumberOfActiveDomains);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfActiveDomains, int);

  static NAN_METHOD(GetNumberOfActiveInterfaces);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfActiveInterfaces, int);

  static NAN_METHOD(GetNumberOfActiveNetworks);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfActiveNetworks, int);

  static NAN_METHOD(GetNumberOfNetworkFilters);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfActiveStoragePools, int);

  static NAN_METHOD(GetNumberOfSecrets);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfNetworkFilters, int);

  static NAN_METHOD(GetNumberOfActiveStoragePools);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNumberOfSecrets, int);

  // Node functions
  static NAN_METHOD(ListNodeDevices);
  static NAN_METHOD(GetNodeSecurityModel);
  static NAN_METHOD(GetNodeInfo);

  static NAN_METHOD(GetNodeFreeMemory);
  HYPERVISOR_PRIMITIVE_RETURN_WORKER(GetNodeFreeMemory, double);

  static NAN_METHOD(GetNodeCellsFreeMemory);
  class GetNodeCellsFreeMemoryWorker : public ListReturnWorker<double, v8::Number> {
  public:
    GetNodeCellsFreeMemoryWorker(NanCallback *callback, Hypervisor *hypervisor, int startCell, int maxCells)
      : ListReturnWorker(callback, hypervisor->conn_), startCell_(startCell), maxCells_(maxCells) {}
    void Execute();
  private:
    int startCell_;
    int maxCells_;
  };

  // Event functions
  // static NAN_METHOD(RegisterDomainEvent);
  // static NAN_METHOD(UnregisterDomainEvent);

  // Misc functions
  // static NAN_METHOD(FindStoragePoolSources);

private:
  class ConnectWorker : public LibVirtWorker {
  public:
    ConnectWorker(NanCallback *callback, Hypervisor *hypervisor)
      : LibVirtWorker(callback, NULL), hypervisor_(hypervisor) {}

    void Execute();
    static int auth_callback(virConnectCredentialPtr cred, unsigned int ncred, void *data);
  private:
    Hypervisor *hypervisor_;
  };

  class GetMaxVcpusWorker : public PrimitiveReturnWorker<int> {
  public:
    GetMaxVcpusWorker(NanCallback *callback, Hypervisor *hypervisor, const std::string &type)
      : PrimitiveReturnWorker(callback, hypervisor->conn_), type_(type) {}
    void Execute();
  private:
    std::string type_;
  };

  class ListNodeDevicesWorker : public ListReturnWorker<std::string, v8::String> {
  public:
    ListNodeDevicesWorker(NanCallback *callback, Hypervisor *hypervisor, const std::string &cap)
      : ListReturnWorker(callback, hypervisor->conn_), capability_(cap) {}
    void Execute();
  private:
    std::string capability_;
  };

  class SetKeepAliveWorker : public PrimitiveReturnWorker<bool> {
  public:
    SetKeepAliveWorker(NanCallback *callback, Hypervisor *hypervisor, int interval, unsigned int count)
      : PrimitiveReturnWorker(callback, hypervisor->conn_), interval_(interval), count_(count) {}
    void Execute();
  private:
    int interval_;
    unsigned int count_;
  };

  class GetBaselineCPUWorker : public PrimitiveReturnWorker<std::string> {
  public:
    GetBaselineCPUWorker(NanCallback *callback, Hypervisor *hypervisor, char **cpus, int count, int flags)
      : PrimitiveReturnWorker(callback, hypervisor->conn_),
        cpus_(cpus), count_(count), flags_(flags) {}
    void Execute();
  private:
    char **cpus_;
    int count_;
    int flags_;
  };

  class CompareCPUWorker : public PrimitiveReturnWorker<int> {
  public:
    CompareCPUWorker(NanCallback *callback, Hypervisor *hypervisor, const std::string &cpu, int flags)
      : PrimitiveReturnWorker(callback, hypervisor->conn_),
        cpu_(cpu), flags_(flags) {}
    void Execute();
  private:
    std::string cpu_;
    int flags_;
  };

  class GetNodeSecurityModelWorker : public LibVirtWorker {
  public:
    GetNodeSecurityModelWorker(NanCallback *callback, Hypervisor *hypervisor)
      : LibVirtWorker(callback, hypervisor->conn_) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    virSecurityModel securityModel_;
  };

  class GetNodeInfoWorker : public LibVirtWorker {
  public:
    GetNodeInfoWorker(NanCallback *callback, Hypervisor *hypervisor)
      : LibVirtWorker(callback, hypervisor->conn_) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    virNodeInfo info_;
  };

/*
  static void domain_event_free(void *opaque);
  static int domain_event_lifecycle_callback( virConnectPtr conn,
                                              virDomainPtr domain,
                                              int event,
                                              int detail,
                                              void *opaque);
  static int domain_event_generic_callback(   virConnectPtr conn,
                                              virDomainPtr domain,
                                              void *opaque);
  static int domain_event_rtcchange_callback( virConnectPtr conn,
                                              virDomainPtr domain,
                                              long long utcoffset,
                                              void *opaque);
  static int domain_event_watchdog_callback(  virConnectPtr conn,
                                              virDomainPtr domain,
                                              int action,
                                              void *opaque);
  static int domain_event_io_error_callback(  virConnectPtr conn,
                                              virDomainPtr domain,
                                              const char *src_path,
                                              const char *dev_alias,
                                              int action,
                                              void *opaque);
  static int domain_event_io_error_reason_callback(virConnectPtr conn,
                                                   virDomainPtr domain,
                                                   const char *src_path,
                                                   const char *dev_alias,
                                                   int action,
                                                   const char *reason,
                                                   void *opaque);
  static int domain_event_graphics_callback(  virConnectPtr conn,
                                              virDomainPtr domain,
                                              int phase,
                                              virDomainEventGraphicsAddressPtr local,
                                              virDomainEventGraphicsAddressPtr remote,
                                              const char *authScheme,
                                              virDomainEventGraphicsSubjectPtr subject,
                                              void *opaque);
  static int auth_callback(   virConnectCredentialPtr cred,
                              unsigned int ncred,
                              void *data);
  */
};

}  // namespace NodeLibvirt

#endif  // SRC_HYPERVISOR_H_
