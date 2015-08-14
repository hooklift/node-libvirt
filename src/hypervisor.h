// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_HYPERVISOR_H_
#define SRC_HYPERVISOR_H_

#include <string>

#include "node_libvirt.h"
#include "domain.h"

#include "nlv_async_worker.h"
#include "worker_macros.h"

namespace NodeLibvirt {

class Hypervisor : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);

  virConnectPtr Connection() const;

private:
  explicit Hypervisor(std::string uri, std::string user, std::string pass, bool readOnly);
  virtual ~Hypervisor();

  static Persistent<Function> constructor;
  static Persistent<FunctionTemplate> constructor_template;

  virConnectPtr handle_;
  std::string uri_;
  std::string username_;
  std::string password_;
  bool readOnly_;

  friend class Interface;
  friend class Network;
  friend class NetworkFilter;
  friend class NodeDevice;
  friend class Secret;
  friend class StorageVolume;
  friend class StoragePool;
  friend class Domain;

private:
  // ACTIONS
  static NAN_METHOD(New);
  static NAN_METHOD(Connect);
  static NAN_METHOD(Disconnect);
  static NAN_METHOD(CompareCPU);

  // ACCESSORS/MUTATORS
  static NAN_METHOD(GetCapabilities);
  static NAN_METHOD(GetHostname);
  static NAN_METHOD(GetSysInfo);
  static NAN_METHOD(GetType);
  static NAN_METHOD(GetConnectionUri);
  static NAN_METHOD(GetVersion);
  static NAN_METHOD(GetLibVirtVersion);
  static NAN_METHOD(IsConnectionEncrypted);
  static NAN_METHOD(IsConnectionSecure);
  static NAN_METHOD(IsConnectionAlive);
  static NAN_METHOD(GetMaxVcpus);
  static NAN_METHOD(SetKeepAlive);
  static NAN_METHOD(GetBaselineCPU);

  static NAN_METHOD(ListDefinedDomains);
  static NAN_METHOD(ListDefinedNetworks);
  static NAN_METHOD(ListDefinedStoragePools);
  static NAN_METHOD(ListDefinedInterfaces);
  static NAN_METHOD(ListActiveDomains);
  static NAN_METHOD(ListActiveInterfaces);
  static NAN_METHOD(ListNetworkFilters);
  static NAN_METHOD(ListActiveNetworks);
  static NAN_METHOD(ListSecrets);
  static NAN_METHOD(ListActiveStoragePools);

  static NAN_METHOD(GetNumberOfDefinedDomains);
  static NAN_METHOD(GetNumberOfDefinedInterfaces);
  static NAN_METHOD(GetNumberOfDefinedNetworks);
  static NAN_METHOD(GetNumberOfDefinedStoragePools);
  static NAN_METHOD(GetNumberOfActiveDomains);
  static NAN_METHOD(GetNumberOfActiveInterfaces);
  static NAN_METHOD(GetNumberOfActiveNetworks);
  static NAN_METHOD(GetNumberOfNetworkFilters);
  static NAN_METHOD(GetNumberOfSecrets);
  static NAN_METHOD(GetNumberOfActiveStoragePools);

  static NAN_METHOD(ListNodeDevices);
  static NAN_METHOD(GetNodeSecurityModel);
  static NAN_METHOD(GetNodeInfo);
  static NAN_METHOD(GetNodeFreeMemory);
  static NAN_METHOD(GetNodeMemoryStats);
  static NAN_METHOD(GetNodeCellsFreeMemory);

  // Misc functions
  // static NAN_METHOD(FindStoragePoolSources);

private:
  // ACTION WORKERS
  class ConnectWorker : public NLVAsyncWorker<virConnectPtr> {
  public:
    ConnectWorker(NanCallback *callback, Hypervisor *hypervisor)
      : NLVAsyncWorker(callback, NULL), hypervisor_(hypervisor) {}

    void Execute();
    static int auth_callback(virConnectCredentialPtr cred, unsigned int ncred, void *data);
  private:
    Hypervisor *hypervisor_;
  };

  class DisconnectWorker : public NLVAsyncWorker<virConnectPtr> {
  public:
    DisconnectWorker(NanCallback *callback, Hypervisor *hypervisor)
      : NLVAsyncWorker(callback, hypervisor->handle_), hypervisor_(hypervisor) {}
    void Execute();
  private:
    Hypervisor *hypervisor_;
  };

  class CompareCPUWorker : public NLVPrimitiveReturnWorker<virConnectPtr, int> {
  public:
    CompareCPUWorker(NanCallback *callback, virConnectPtr handle, const std::string &cpu, int flags)
      : NLVPrimitiveReturnWorker(callback, handle),
        cpu_(cpu), flags_(flags) {}
    void Execute();
  private:
    std::string cpu_;
    int flags_;
  };

  // ACCESSOR/MUTATOR WORKERS
  NLV_PRIMITIVE_RETURN_WORKER2(GetCapabilities, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetHostname, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetSysInfo, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetType, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetConnectionUri, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetVersion, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(GetLibVirtVersion, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER2(IsConnectionEncrypted, virConnectPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER2(IsConnectionSecure, virConnectPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER2(IsConnectionAlive, virConnectPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER2(GetMaxVcpusWorker, virConnectPtr, int);

  NLV_LIST_RETURN_WORKER2(ListDefinedDomains, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER2(ListDefinedNetworks, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER2(ListDefinedStoragePools, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER2(ListDefinedInterfaces, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER2(ListActiveDomains, virConnectPtr, int, v8::Integer);
  NLV_LIST_RETURN_WORKER2(ListActiveInterfaces, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER2(ListNetworkFilters, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER2(ListActiveNetworks, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER2(ListSecrets, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER2(ListActiveStoragePools, virConnectPtr, std::string, v8::String);

  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfDefinedDomains, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfDefinedInterfaces, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfDefinedNetworks, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfDefinedStoragePools, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfActiveDomains, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfActiveInterfaces, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfActiveNetworks, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfActiveStoragePools, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfNetworkFilters, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER2(GetNumberOfSecrets, virConnectPtr, int);

  NLV_PRIMITIVE_RETURN_WORKER2(GetNodeFreeMemory, virConnectPtr, double);

  class GetNodeCellsFreeMemoryWorker : public NLVListReturnWorker<virConnectPtr, double, v8::Number> {
  public:
    GetNodeCellsFreeMemoryWorker(NanCallback *callback, virConnectPtr handle, int startCell, int maxCells)
      : NLVListReturnWorker(callback, handle), startCell_(startCell), maxCells_(maxCells) {}
    void Execute();
  private:
    int startCell_;
    int maxCells_;
  };

  class GetMaxVcpusWorker : public NLVPrimitiveReturnWorker<virConnectPtr, int> {
  public:
    GetMaxVcpusWorker(NanCallback *callback, virConnectPtr handle, const std::string &type)
      : NLVPrimitiveReturnWorker(callback, handle), type_(type) {}
    void Execute();
  private:
    std::string type_;
  };

  class ListNodeDevicesWorker : public NLVListReturnWorker<virConnectPtr, std::string, v8::String> {
  public:
    ListNodeDevicesWorker(NanCallback *callback, virConnectPtr handle, const std::string &cap)
      : NLVListReturnWorker(callback, handle), capability_(cap) {}
    void Execute();
  private:
    std::string capability_;
  };

  class SetKeepAliveWorker : public NLVPrimitiveReturnWorker<virConnectPtr, bool> {
  public:
    SetKeepAliveWorker(NanCallback *callback, virConnectPtr handle, int interval, unsigned int count)
      : NLVPrimitiveReturnWorker(callback, handle), interval_(interval), count_(count) {}
    void Execute();
  private:
    int interval_;
    unsigned int count_;
  };

  class GetBaselineCPUWorker : public NLVPrimitiveReturnWorker<virConnectPtr, std::string> {
  public:
    GetBaselineCPUWorker(NanCallback *callback, virConnectPtr handle, char **cpus, int count, int flags)
      : NLVPrimitiveReturnWorker(callback, handle),
        cpus_(cpus), count_(count), flags_(flags) {}
    void Execute();
  private:
    char **cpus_;
    int count_;
    int flags_;
  };

  class GetNodeSecurityModelWorker : public NLVAsyncWorker<virConnectPtr> {
  public:
    GetNodeSecurityModelWorker(NanCallback *callback, virConnectPtr handle)
      : NLVAsyncWorker(callback, handle) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    virSecurityModel securityModel_;
  };

  class GetNodeMemoryStatsWorker : public NLVListReturnWorker<virConnectPtr, double, v8::Number> {
    using NLVAsyncWorker<virConnectPtr>::callback;
  public:
    GetNodeMemoryStatsWorker(NanCallback *callback, virConnectPtr handle, int numCell, int flags)
      : NLVListReturnWorker(callback, handle), cellNum_(numCell), flags_(flags) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    std::vector<virNodeMemoryStats> info_;
    int cellNum_;
    int flags_;
  };

  class GetNodeInfoWorker : public NLVAsyncWorker<virConnectPtr> {
  public:
    GetNodeInfoWorker(NanCallback *callback, virConnectPtr handle)
      : NLVAsyncWorker(callback, handle) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    virNodeInfo info_;
  };

};

}  // namespace NodeLibvirt

#endif  // SRC_HYPERVISOR_H_
