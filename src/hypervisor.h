// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef HYPERVISOR_H
#define HYPERVISOR_H

#include <string>

#include "nlv_object.h"
#include "nlv_async_worker.h"
#include "worker_macros.h"

namespace NLV {

struct HypervisorCleanupHandler {
  static int cleanup(virConnectPtr handle) {
    return virConnectClose(handle);
  }
};

class Hypervisor : public NLVObject<virConnectPtr, HypervisorCleanupHandler>
{
public:
  static void Initialize(Handle<Object> exports);

private:
  explicit Hypervisor(std::string uri, std::string user, std::string pass, bool readOnly);
  static Nan::Persistent<Function> constructor;
  static Nan::Persistent<FunctionTemplate> constructor_template;

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
    ConnectWorker(Nan::Callback *callback, Hypervisor *hypervisor)
      : NLVAsyncWorker(callback, NULL), hypervisor_(hypervisor) {}

    void Execute();
    static int auth_callback(virConnectCredentialPtr cred, unsigned int ncred, void *data);
  private:
    Hypervisor *hypervisor_;
  };

  class DisconnectWorker : public NLVAsyncWorker<virConnectPtr> {
  public:
    DisconnectWorker(Nan::Callback *callback, Hypervisor *hypervisor)
      : NLVAsyncWorker(callback, hypervisor->handle_), hypervisor_(hypervisor) {}
    void Execute();
  private:
    Hypervisor *hypervisor_;
  };

  class CompareCPUWorker : public NLVPrimitiveReturnWorker<virConnectPtr, int> {
  public:
    CompareCPUWorker(Nan::Callback *callback, virConnectPtr handle, const std::string &cpu, int flags)
      : NLVPrimitiveReturnWorker(callback, handle),
        cpu_(cpu), flags_(flags) {}
    void Execute();
  private:
    std::string cpu_;
    int flags_;
  };

  // ACCESSOR/MUTATOR WORKERS
  NLV_STRING_RETURN_WORKER(GetCapabilities, virConnectPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetHostname, virConnectPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetSysInfo, virConnectPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetType, virConnectPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetConnectionUri, virConnectPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetVersion, virConnectPtr, std::string);
  NLV_STRING_RETURN_WORKER(GetLibVirtVersion, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(IsConnectionEncrypted, virConnectPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsConnectionSecure, virConnectPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsConnectionAlive, virConnectPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetMaxVcpusWorker, virConnectPtr, int);

  NLV_STRING_LIST_RETURN_WORKER(ListDefinedDomains, virConnectPtr, std::string, v8::String);
  NLV_STRING_LIST_RETURN_WORKER(ListDefinedNetworks, virConnectPtr, std::string, v8::String);
  NLV_STRING_LIST_RETURN_WORKER(ListDefinedStoragePools, virConnectPtr, std::string, v8::String);
  NLV_STRING_LIST_RETURN_WORKER(ListDefinedInterfaces, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER(ListActiveDomains, virConnectPtr, int, v8::Integer);
  NLV_STRING_LIST_RETURN_WORKER(ListActiveInterfaces, virConnectPtr, std::string, v8::String);
  NLV_STRING_LIST_RETURN_WORKER(ListNetworkFilters, virConnectPtr, std::string, v8::String);
  NLV_STRING_LIST_RETURN_WORKER(ListActiveNetworks, virConnectPtr, std::string, v8::String);
  NLV_STRING_LIST_RETURN_WORKER(ListSecrets, virConnectPtr, std::string, v8::String);
  NLV_STRING_LIST_RETURN_WORKER(ListActiveStoragePools, virConnectPtr, std::string, v8::String);

  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfDefinedDomains, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfDefinedInterfaces, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfDefinedNetworks, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfDefinedStoragePools, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfActiveDomains, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfActiveInterfaces, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfActiveNetworks, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfActiveStoragePools, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfNetworkFilters, virConnectPtr, int);
  NLV_PRIMITIVE_RETURN_WORKER(GetNumberOfSecrets, virConnectPtr, int);

  NLV_PRIMITIVE_RETURN_WORKER(GetNodeFreeMemory, virConnectPtr, double);

  class GetNodeCellsFreeMemoryWorker : public NLVListReturnWorker<virConnectPtr, double, v8::Number> {
  public:
    GetNodeCellsFreeMemoryWorker(Nan::Callback *callback, virConnectPtr handle, int startCell, int maxCells)
      : NLVListReturnWorker(callback, handle), startCell_(startCell), maxCells_(maxCells) {}
    void Execute();
  private:
    int startCell_;
    int maxCells_;
  };

  class GetMaxVcpusWorker : public NLVPrimitiveReturnWorker<virConnectPtr, int> {
  public:
    GetMaxVcpusWorker(Nan::Callback *callback, virConnectPtr handle, const std::string &type)
      : NLVPrimitiveReturnWorker(callback, handle), type_(type) {}
    void Execute();
  private:
    std::string type_;
  };

  class ListNodeDevicesWorker : public NLVStringListReturnWorker<virConnectPtr, std::string, v8::String> {
  public:
    ListNodeDevicesWorker(Nan::Callback *callback, virConnectPtr handle, const std::string &cap)
      : NLVStringListReturnWorker(callback, handle), capability_(cap) {}
    void Execute();
  private:
    std::string capability_;
  };

  class SetKeepAliveWorker : public NLVPrimitiveReturnWorker<virConnectPtr, bool> {
  public:
    SetKeepAliveWorker(Nan::Callback *callback, virConnectPtr handle, int interval, unsigned int count)
      : NLVPrimitiveReturnWorker(callback, handle), interval_(interval), count_(count) {}
    void Execute();
  private:
    int interval_;
    unsigned int count_;
  };

  class GetBaselineCPUWorker : public NLVStringReturnWorker<virConnectPtr, std::string> {
  public:
    GetBaselineCPUWorker(Nan::Callback *callback, virConnectPtr handle, char **cpus, int count, int flags)
      : NLVStringReturnWorker(callback, handle),
        cpus_(cpus), count_(count), flags_(flags) {}
    void Execute();
  private:
    char **cpus_;
    int count_;
    int flags_;
  };

  class GetNodeSecurityModelWorker : public NLVAsyncWorker<virConnectPtr> {
  public:
    GetNodeSecurityModelWorker(Nan::Callback *callback, virConnectPtr handle)
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
    GetNodeMemoryStatsWorker(Nan::Callback *callback, virConnectPtr handle, int numCell, int flags)
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
    GetNodeInfoWorker(Nan::Callback *callback, virConnectPtr handle)
      : NLVAsyncWorker(callback, handle) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    virNodeInfo info_;
  };

};

}  // namespace NLV

#endif  // HYPERVISOR_H
