// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_HYPERVISOR_H_
#define SRC_HYPERVISOR_H_

#include "node_libvirt.h"

#include "worker_macros.h"
#include "worker.h"

namespace NodeLibvirt {

class Hypervisor : public ObjectWrap
{
public:
  static void Initialize(Handle<Object> exports);

  virConnectPtr Connection() const;

private:
  explicit Hypervisor(string uri, string user, string pass, bool readOnly);
  virtual ~Hypervisor();

  static Persistent<Function> constructor;
  static Persistent<FunctionTemplate> constructor_template;

  virConnectPtr handle_;
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
  static NAN_METHOD(GetNodeCellsFreeMemory);

  // Misc functions
  // static NAN_METHOD(FindStoragePoolSources);

private:
  // ACTION WORKERS
  class ConnectWorker : public LibVirtWorker {
  public:
    ConnectWorker(NanCallback *callback, Hypervisor *hypervisor)
      : LibVirtWorker(callback, LibVirtHandle()), hypervisor_(hypervisor) {}

    void Execute();
    static int auth_callback(virConnectCredentialPtr cred, unsigned int ncred, void *data);
  private:
    Hypervisor *hypervisor_;
  };

  class DisconnectWorker : public LibVirtWorker {
  public:
    DisconnectWorker(NanCallback *callback, const LibVirtHandle &handle)
      : LibVirtWorker(callback, handle) {}
    void Execute();
  };

  class CompareCPUWorker : public PrimitiveReturnWorker<int> {
  public:
    CompareCPUWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &cpu, int flags)
      : PrimitiveReturnWorker(callback, handle),
        cpu_(cpu), flags_(flags) {}
    void Execute();
  private:
    std::string cpu_;
    int flags_;
  };

  // ACCESSOR/MUTATOR WORKERS
  NLV_PRIMITIVE_RETURN_WORKER(GetCapabilities, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetHostname, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetSysInfo, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetType, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetConnectionUri, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetVersion, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(GetLibVirtVersion, virConnectPtr, std::string);
  NLV_PRIMITIVE_RETURN_WORKER(IsConnectionEncrypted, virConnectPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsConnectionSecure, virConnectPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(IsConnectionAlive, virConnectPtr, bool);
  NLV_PRIMITIVE_RETURN_WORKER(GetMaxVcpusWorker, virConnectPtr, int);

  NLV_LIST_RETURN_WORKER(ListDefinedDomains, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER(ListDefinedNetworks, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER(ListDefinedStoragePools, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER(ListDefinedInterfaces, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER(ListActiveDomains, virConnectPtr, int, v8::Integer);
  NLV_LIST_RETURN_WORKER(ListActiveInterfaces, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER(ListNetworkFilters, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER(ListActiveNetworks, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER(ListSecrets, virConnectPtr, std::string, v8::String);
  NLV_LIST_RETURN_WORKER(ListActiveStoragePools, virConnectPtr, std::string, v8::String);

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

  class GetNodeCellsFreeMemoryWorker : public ListReturnWorker<double, v8::Number> {
  public:
    GetNodeCellsFreeMemoryWorker(NanCallback *callback, const LibVirtHandle &handle, int startCell, int maxCells)
      : ListReturnWorker(callback, handle), startCell_(startCell), maxCells_(maxCells) {}
    void Execute();
  private:
    int startCell_;
    int maxCells_;
  };

  class GetMaxVcpusWorker : public PrimitiveReturnWorker<int> {
  public:
    GetMaxVcpusWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &type)
      : PrimitiveReturnWorker(callback, handle), type_(type) {}
    void Execute();
  private:
    std::string type_;
  };

  class ListNodeDevicesWorker : public ListReturnWorker<std::string, v8::String> {
  public:
    ListNodeDevicesWorker(NanCallback *callback, const LibVirtHandle &handle, const std::string &cap)
      : ListReturnWorker(callback, handle), capability_(cap) {}
    void Execute();
  private:
    std::string capability_;
  };

  class SetKeepAliveWorker : public PrimitiveReturnWorker<bool> {
  public:
    SetKeepAliveWorker(NanCallback *callback, const LibVirtHandle &handle, int interval, unsigned int count)
      : PrimitiveReturnWorker(callback, handle), interval_(interval), count_(count) {}
    void Execute();
  private:
    int interval_;
    unsigned int count_;
  };

  class GetBaselineCPUWorker : public PrimitiveReturnWorker<std::string> {
  public:
    GetBaselineCPUWorker(NanCallback *callback, const LibVirtHandle &handle, char **cpus, int count, int flags)
      : PrimitiveReturnWorker(callback, handle),
        cpus_(cpus), count_(count), flags_(flags) {}
    void Execute();
  private:
    char **cpus_;
    int count_;
    int flags_;
  };

  class GetNodeSecurityModelWorker : public LibVirtWorker {
  public:
    GetNodeSecurityModelWorker(NanCallback *callback, const LibVirtHandle &handle)
      : LibVirtWorker(callback, handle) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    virSecurityModel securityModel_;
  };

  class GetNodeInfoWorker : public LibVirtWorker {
  public:
    GetNodeInfoWorker(NanCallback *callback, const LibVirtHandle &handle)
      : LibVirtWorker(callback, handle) {}
    void Execute();
  protected:
    void HandleOKCallback();
  private:
    virNodeInfo info_;
  };

};

}  // namespace NodeLibvirt

#endif  // SRC_HYPERVISOR_H_
