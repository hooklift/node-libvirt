// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

#include <assert.h>

#include "worker_macros.h"
#include "node_libvirt.h"
#include "domain.h"
#include "node_device.h"
#include "secret.h"
#include "storage_pool.h"
#include "storage_volume.h"
#include "network.h"
#include "network_filter.h"
#include "interface.h"
#include "hypervisor.h"

namespace NLV {

Nan::Persistent<Function> Hypervisor::constructor;
Nan::Persistent<FunctionTemplate> Hypervisor::constructor_template;
void Hypervisor::Initialize(Handle<Object> exports)
{
  Nan::HandleScope scope;

  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
  t->SetClassName(Nan::New("Hypervisor").ToLocalChecked());
  t->InstanceTemplate()->SetInternalFieldCount(1);

  // methods
  Nan::SetPrototypeMethod(t, "connect",                 Connect);
  Nan::SetPrototypeMethod(t, "disconnect",              Disconnect);

  Nan::SetPrototypeMethod(t, "getCapabilities",         GetCapabilities);
  Nan::SetPrototypeMethod(t, "getHostname",             GetHostname);
  Nan::SetPrototypeMethod(t, "getSysInfo",              GetSysInfo);
  Nan::SetPrototypeMethod(t, "getType",                 GetType);
  Nan::SetPrototypeMethod(t, "getConnectionUri",        GetConnectionUri);
  Nan::SetPrototypeMethod(t, "getVersion",              GetVersion);
  Nan::SetPrototypeMethod(t, "getLibVirtVersion",       GetLibVirtVersion);
  Nan::SetPrototypeMethod(t, "isConnectionEncrypted",   IsConnectionEncrypted);
  Nan::SetPrototypeMethod(t, "isConnectionSecure",      IsConnectionSecure);
  Nan::SetPrototypeMethod(t, "isConnectionAlive",       IsConnectionAlive);
  Nan::SetPrototypeMethod(t, "getMaxVcpus",             GetMaxVcpus);
  Nan::SetPrototypeMethod(t, "setKeepAlive",            SetKeepAlive);
  Nan::SetPrototypeMethod(t, "getBaselineCPU",          GetBaselineCPU);
  Nan::SetPrototypeMethod(t, "compareCPU",              CompareCPU);

  Nan::SetPrototypeMethod(t, "listDefinedDomains",      ListDefinedDomains);
  Nan::SetPrototypeMethod(t, "listDefinedNetworks",     ListDefinedNetworks);
  Nan::SetPrototypeMethod(t, "listDefinedStoragePools", ListDefinedStoragePools);
  Nan::SetPrototypeMethod(t, "listDefinedInterfaces",   ListDefinedInterfaces);
  Nan::SetPrototypeMethod(t, "listActiveDomains",       ListActiveDomains);
  Nan::SetPrototypeMethod(t, "listActiveInterfaces",    ListActiveInterfaces);
  Nan::SetPrototypeMethod(t, "listActiveNetworks",      ListActiveNetworks);
  Nan::SetPrototypeMethod(t, "listActiveStoragePools",  ListActiveStoragePools);
  Nan::SetPrototypeMethod(t, "listNetworkFilters",      ListNetworkFilters);
  Nan::SetPrototypeMethod(t, "listSecrets",             ListSecrets);

  Nan::SetPrototypeMethod(t, "getNumberOfDefinedDomains", GetNumberOfDefinedDomains);
  Nan::SetPrototypeMethod(t, "getNumberOfDefinedInterfaces", GetNumberOfDefinedInterfaces);
  Nan::SetPrototypeMethod(t, "getNumberOfDefinedNetworks", GetNumberOfDefinedNetworks);
  Nan::SetPrototypeMethod(t, "getNumberOfDefinedStoragePools", GetNumberOfDefinedStoragePools);
  Nan::SetPrototypeMethod(t, "getNumberOfActiveDomains", GetNumberOfActiveDomains);
  Nan::SetPrototypeMethod(t, "getNumberOfActiveInterfaces", GetNumberOfActiveInterfaces);
  Nan::SetPrototypeMethod(t, "getNumberOfActiveNetworks", GetNumberOfActiveNetworks);
  Nan::SetPrototypeMethod(t, "getNumberOfNetworkFilters", GetNumberOfNetworkFilters);
  Nan::SetPrototypeMethod(t, "getNumberOfSecrets", GetNumberOfSecrets);
  Nan::SetPrototypeMethod(t, "getNumberOfActiveStoragePools", GetNumberOfActiveStoragePools);

  // NODE
  Nan::SetPrototypeMethod(t, "listNodeDevices",             ListNodeDevices);
  Nan::SetPrototypeMethod(t, "getNodeSecurityModel",        GetNodeSecurityModel);
  Nan::SetPrototypeMethod(t, "getNodeInfo",                 GetNodeInfo);
  Nan::SetPrototypeMethod(t, "getNodeFreeMemory",           GetNodeFreeMemory);
  Nan::SetPrototypeMethod(t, "getNodeMemoryStats",          GetNodeMemoryStats);
  Nan::SetPrototypeMethod(t, "getNodeCellsFreeMemory",      GetNodeCellsFreeMemory);

  // INTERFACE
  Nan::SetPrototypeMethod(t, "lookupInterfaceByName",       Interface::LookupByName);
  Nan::SetPrototypeMethod(t, "lookupInterfaceByMacAddress", Interface::LookupByMacAddress);
  Nan::SetPrototypeMethod(t, "defineInterface",             Interface::Define);

  // NETWORK
  Nan::SetPrototypeMethod(t, "createNetwork",               Network::Create);
  Nan::SetPrototypeMethod(t, "lookupNetworkByName",         Network::LookupByName);
  Nan::SetPrototypeMethod(t, "lookupNetworkByUUID",         Network::LookupByUUID);
  Nan::SetPrototypeMethod(t, "defineNetwork",               Network::Define);

  // NETWORK FILTER
  Nan::SetPrototypeMethod(t, "defineNetworkFilter",         NetworkFilter::Define);
  Nan::SetPrototypeMethod(t, "lookupNetworkFilterByName",   NetworkFilter::LookupByName);
  Nan::SetPrototypeMethod(t, "lookupNetworkFilterByUUID",   NetworkFilter::LookupByUUID);

  // STORAGE POOL
  Nan::SetPrototypeMethod(t, "createStoragePool",           StoragePool::Create);
  Nan::SetPrototypeMethod(t, "defineStoragePool",           StoragePool::Define);
  Nan::SetPrototypeMethod(t, "lookupStoragePoolByName",     StoragePool::LookupByName);
  Nan::SetPrototypeMethod(t, "lookupStoragePoolByUUID",     StoragePool::LookupByUUID);
  Nan::SetPrototypeMethod(t, "lookupStoragePoolByVolume",   StoragePool::LookupByVolume);

  // STORAGE VOLUME
  Nan::SetPrototypeMethod(t, "lookupStorageVolumeByKey",    StorageVolume::LookupByKey);
  Nan::SetPrototypeMethod(t, "lookupStorageVolumeByPath",   StorageVolume::LookupByPath);

  // SECRET
  Nan::SetPrototypeMethod(t, "defineSecret",                Secret::Define);
  Nan::SetPrototypeMethod(t, "lookupSecretByUsage",         Secret::LookupByUsage);
  Nan::SetPrototypeMethod(t, "lookupSecretByUUID",          Secret::LookupByUUID);

  // NODE
  Nan::SetPrototypeMethod(t, "lookupNodeDeviceByName",      NodeDevice::LookupByName);
  Nan::SetPrototypeMethod(t, "createNodeDevice",            NodeDevice::Create);

  // DOMAIN
  Nan::SetPrototypeMethod(t, "createDomain",                Domain::Create);
  Nan::SetPrototypeMethod(t, "defineDomain",                Domain::Define);
  Nan::SetPrototypeMethod(t, "restoreDomain",               Domain::Restore);
  Nan::SetPrototypeMethod(t, "lookupDomainById",            Domain::LookupById);
  Nan::SetPrototypeMethod(t, "lookupDomainByName",          Domain::LookupByName);
  Nan::SetPrototypeMethod(t, "lookupDomainByUUID",          Domain::LookupByUUID);

  //Constants initialization
  // virConnectCredentialType
  NODE_DEFINE_CONSTANT(exports, VIR_CRED_USERNAME);
  NODE_DEFINE_CONSTANT(exports, VIR_CRED_AUTHNAME);
  NODE_DEFINE_CONSTANT(exports, VIR_CRED_LANGUAGE);
  NODE_DEFINE_CONSTANT(exports, VIR_CRED_CNONCE);
  NODE_DEFINE_CONSTANT(exports, VIR_CRED_PASSPHRASE);
  NODE_DEFINE_CONSTANT(exports, VIR_CRED_ECHOPROMPT);
  NODE_DEFINE_CONSTANT(exports, VIR_CRED_NOECHOPROMPT);
  NODE_DEFINE_CONSTANT(exports, VIR_CRED_REALM);
  NODE_DEFINE_CONSTANT(exports, VIR_CRED_EXTERNAL);

  // virCPUCompareResult
  NODE_DEFINE_CONSTANT(exports, VIR_CPU_COMPARE_ERROR);
  NODE_DEFINE_CONSTANT(exports, VIR_CPU_COMPARE_INCOMPATIBLE);
  NODE_DEFINE_CONSTANT(exports, VIR_CPU_COMPARE_IDENTICAL);
  NODE_DEFINE_CONSTANT(exports, VIR_CPU_COMPARE_SUPERSET);

  // virSecretUsageType
  NODE_DEFINE_CONSTANT(exports, VIR_SECRET_USAGE_TYPE_NONE);
  NODE_DEFINE_CONSTANT(exports, VIR_SECRET_USAGE_TYPE_VOLUME);

  // virDomainEventID
  NODE_DEFINE_CONSTANT(exports, VIR_DOMAIN_EVENT_ID_LIFECYCLE);
  NODE_DEFINE_CONSTANT(exports, VIR_DOMAIN_EVENT_ID_REBOOT);
  NODE_DEFINE_CONSTANT(exports, VIR_DOMAIN_EVENT_ID_RTC_CHANGE);
  NODE_DEFINE_CONSTANT(exports, VIR_DOMAIN_EVENT_ID_WATCHDOG);
  NODE_DEFINE_CONSTANT(exports, VIR_DOMAIN_EVENT_ID_IO_ERROR);
  NODE_DEFINE_CONSTANT(exports, VIR_DOMAIN_EVENT_ID_GRAPHICS);
  NODE_DEFINE_CONSTANT(exports, VIR_DOMAIN_EVENT_ID_IO_ERROR_REASON);

  // virMemory
  NODE_DEFINE_CONSTANT(exports, VIR_NODE_MEMORY_STATS_ALL_CELLS);

#ifdef VIR_ENUM_SENTINELS
  NODE_DEFINE_CONSTANT(exports, VIR_DOMAIN_EVENT_ID_LAST);
#endif

  t->SetClassName(Nan::New("Hypervisor").ToLocalChecked());
  constructor_template.Reset(t);
  constructor.Reset(t->GetFunction());
  exports->Set(Nan::New("Hypervisor").ToLocalChecked(), t->GetFunction());
}

Hypervisor::Hypervisor(std::string uri, std::string username, std::string password, bool readonly)
  : NLVObject(NULL),
    uri_(uri),
    username_(username),
    password_(password),
    readOnly_(readonly)
{
}

NAN_METHOD(Hypervisor::New)
{
  Nan::HandleScope scope;

  if (!info.IsConstructCall()) {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { info[0] };
    Local<Function> cons = Nan::New<Function>(constructor);
    return info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }

  int infoLen = info.Length();
  if (infoLen == 0) {
    Nan::ThrowTypeError("You must specify a connection URI as first argument");
    return;
  }

  if (infoLen == 1 && !info[0]->IsString()) {
    Nan::ThrowTypeError("You must specify a string as connection URI");
    return;
  }

  bool readOnly = false;
  std::string username;
  std::string password;
  std::string uri = *Nan::Utf8String(info[0]);

  if (infoLen >= 2) {
    if (info[1]->IsBoolean()) {
      readOnly = info[1]->IsTrue();
    } else if (info[1]->IsObject()) {
      Local<Object> options = info[1].As<Object>();

      if (options->Has(Nan::New("readOnly").ToLocalChecked()))
        readOnly = options->Get(Nan::New("readOnly").ToLocalChecked())->BooleanValue();

      if (options->Has(Nan::New("username").ToLocalChecked()))
        username = *Nan::Utf8String(options->Get(Nan::New("username").ToLocalChecked()));

      if (options->Has(Nan::New("password").ToLocalChecked()))
        password = *Nan::Utf8String(options->Get(Nan::New("password").ToLocalChecked()));
    }
  }

  Hypervisor *hypervisor = new Hypervisor(uri, username, password, readOnly);
  hypervisor->Wrap(info.This());

  // add values to instance object
  info.This()->Set(Nan::New("uri").ToLocalChecked(), Nan::New(uri).ToLocalChecked());
  info.This()->Set(Nan::New("username").ToLocalChecked(), Nan::New(username).ToLocalChecked());
  info.This()->Set(Nan::New("password").ToLocalChecked(), Nan::New(password).ToLocalChecked());
  info.This()->Set(Nan::New("readOnly").ToLocalChecked(), Nan::New(readOnly));

  return info.GetReturnValue().Set(info.This());
}

int Hypervisor::ConnectWorker::auth_callback(virConnectCredentialPtr cred,
                                             unsigned int ncred, void *data)
{
  Hypervisor *hypervisor = static_cast<ConnectWorker*>(data)->hypervisor_;

  for (unsigned int i = 0; i < ncred; ++i) {
    switch (cred[i].type) {
    case VIR_CRED_AUTHNAME:
      cred[i].result = strdup(hypervisor->username_.c_str());
      if (cred[i].result == NULL)
        return -1;
      cred[i].resultlen = strlen(cred[i].result);
      break;

    case VIR_CRED_PASSPHRASE:
      cred[i].result = strdup(hypervisor->password_.c_str());
      if (cred[i].result == NULL)
          return -1;
      cred[i].resultlen = strlen(cred[i].result);
      break;
    }
  }

  return 0;
}

NAN_METHOD(Hypervisor::Connect)
{
  Nan::HandleScope scope;
  if (info.Length() == 1 && !info[0]->IsFunction()) {
    Nan::ThrowTypeError("You must specify a function as first argument");
    return;
  }

  Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());
  Hypervisor *hv = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  Nan::AsyncQueueWorker(new ConnectWorker(callback, hv));
  return;
}

NLV_WORKER_EXECUTE(Hypervisor, Connect)
{
  static int supported_cred_types[] = {
    VIR_CRED_AUTHNAME,
    VIR_CRED_PASSPHRASE,
  };

  virConnectAuth auth;
  auth.credtype = supported_cred_types;
  auth.ncredtype = sizeof(supported_cred_types) / sizeof(int);
  auth.cb = ConnectWorker::auth_callback;
  auth.cbdata = this;

  hypervisor_->handle_ =
    virConnectOpenAuth((const char*) hypervisor_->uri_.c_str(), &auth,
                       hypervisor_->readOnly_ ? VIR_CONNECT_RO : 0);
  if (hypervisor_->handle_ == NULL)
    SetVirError(virGetLastError());
}

NAN_METHOD(Hypervisor::Disconnect)
{
  Nan::HandleScope scope;
  if (info.Length() == 1 && !info[0]->IsFunction()) {
    Nan::ThrowTypeError("You must specify a function as first argument");
    return;
  }

  Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());
  Hypervisor *hv = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  Nan::AsyncQueueWorker(new DisconnectWorker(callback, hv));
  return;
}

NLV_WORKER_EXECUTE(Hypervisor, Disconnect)
{
  NLV_WORKER_ASSERT_CONNECTION();
  hypervisor_->ClearChildren();
  hypervisor_->ClearHandle();
  // int result = virConnectClose(Handle());
  // if (result == -1) {
  //   SetVirError(virGetLastError());
  //   return;
  // }

  // assert(result == 0);
  // hypervisor_->handle_ = NULL;
}

#define HYPERVISOR_STRING_RETURN_EXECUTE(MethodName, Accessor)  \
  void Hypervisor::MethodName##Worker::Execute() {  \
    NLV_WORKER_ASSERT_CONNECTION(); \
    char *result = Accessor(Handle()); \
    if (result == NULL) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = result; \
    free(result); \
  }

#define HYPERVISOR_STRING_RETURN_EXECUTE_NO_FREE(MethodName, Accessor)  \
  void Hypervisor::MethodName##Worker::Execute() {  \
    NLV_WORKER_ASSERT_CONNECTION(); \
    const char *result = Accessor(Handle()); \
    if (result == NULL) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = result; \
  }


NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetCapabilities)
HYPERVISOR_STRING_RETURN_EXECUTE(GetCapabilities, virConnectGetCapabilities)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetHostname)
HYPERVISOR_STRING_RETURN_EXECUTE(GetHostname, virConnectGetHostname)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetSysInfo)
NLV_WORKER_EXECUTE(Hypervisor, GetSysInfo)
{
  NLV_WORKER_ASSERT_CONNECTION();
  char *result = virConnectGetSysinfo(Handle(), 0);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetType)
HYPERVISOR_STRING_RETURN_EXECUTE_NO_FREE(GetType, virConnectGetType)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetConnectionUri)
HYPERVISOR_STRING_RETURN_EXECUTE(GetConnectionUri, virConnectGetURI)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetVersion)
NLV_WORKER_EXECUTE(Hypervisor, GetVersion)
{
  NLV_WORKER_ASSERT_CONNECTION();

  unsigned long version;
  int result = virConnectGetVersion(Handle(), &version);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  } else if (result == 0 && version == 0) {
    return;
  }

  unsigned int major = version / 1000000;
  unsigned int minor = version / 1000;
  unsigned int patch = version % 1000;

  char versionString[10];
  sprintf(versionString, "%d.%d.%d", major, minor, patch);
  fprintf(stderr, "%s", versionString);
  data_ = version;
}

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetLibVirtVersion)
NLV_WORKER_EXECUTE(Hypervisor, GetLibVirtVersion)
{
  NLV_WORKER_ASSERT_CONNECTION();

  unsigned long version;
  int result = virConnectGetLibVersion(Handle(), &version);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  unsigned int major = version / 1000000;
  unsigned int minor = version / 1000;
  unsigned int patch = version % 1000;

  char versionString[10];
  sprintf(versionString, "%d.%d.%d", major, minor, patch);
  data_ = version;
}

#define HYPERVISOR_INT_RETURN_EXECUTE(MethodName, Accessor)  \
  void Hypervisor::MethodName##Worker::Execute() {  \
    NLV_WORKER_ASSERT_CONNECTION(); \
    int result = Accessor(Handle());  \
    if (result == -1) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = result;  \
  }

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, IsConnectionEncrypted)
NLV_BOOL_RETURN_EXECUTE_IMPL(Hypervisor, IsConnectionEncrypted, virConnectIsEncrypted)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, IsConnectionSecure)
NLV_BOOL_RETURN_EXECUTE_IMPL(Hypervisor, IsConnectionSecure, virConnectIsSecure)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, IsConnectionAlive)
NLV_BOOL_RETURN_EXECUTE_IMPL(Hypervisor, IsConnectionAlive, virConnectIsAlive)

NAN_METHOD(Hypervisor::GetMaxVcpus)
{
  Nan::HandleScope scope;

  if (info.Length() < 2) {
    Nan::ThrowTypeError("You must specify a type and callback");
    return;
  }

  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("Type must be a string");
    return;
  }

  if (!info[1]->IsFunction()) {
    Nan::ThrowTypeError("Second argument must be a callback");
    return;
  }

  std::string type(*Nan::Utf8String(info[0]));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Hypervisor *hypervisor = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  Nan::AsyncQueueWorker(new GetMaxVcpusWorker(callback, hypervisor->handle_, type));
  return;
}

NLV_WORKER_EXECUTE(Hypervisor, GetMaxVcpus)
{
  NLV_WORKER_ASSERT_CONNECTION();
  int result = virConnectGetMaxVcpus(Handle(), type_.c_str());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NAN_METHOD(Hypervisor::SetKeepAlive)
{
  Nan::HandleScope scope;

  if (info.Length() < 3) {
    Nan::ThrowTypeError("You must specify an interval, count and callback");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("interval must be a number");
    return;
  }

  if (!info[1]->IsNumber()) {
    Nan::ThrowTypeError("count must be a number");
    return;
  }

  if (!info[2]->IsFunction()) {
    Nan::ThrowTypeError("third argument must be callback");
    return;
  }

  std::string type(*Nan::Utf8String(info[0]));
  int interval = info[0]->IntegerValue();
  unsigned int count = info[1]->IntegerValue();
  Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());
  Hypervisor *hypervisor = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  Nan::AsyncQueueWorker(new SetKeepAliveWorker(callback, hypervisor->handle_, interval, count));
  return;
}

NLV_WORKER_EXECUTE(Hypervisor, SetKeepAlive)
{
  NLV_WORKER_ASSERT_CONNECTION();

  int result = virConnectSetKeepAlive(Handle(), interval_, count_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = !(static_cast<bool>(result));
}

NAN_METHOD(Hypervisor::GetBaselineCPU)
{
  Nan::HandleScope scope;

  if (info.Length() == 0 ||
      (info.Length() < 2 && (!info[0]->IsArray() && !info[1]->IsFunction()))) {
    Nan::ThrowTypeError("You must specify an array with two cpu descriptions and a callback");
    return;
  }

  Local<Array> cpusArguments = info[0].As<Array>();

  int flags = 0;
  int count = cpusArguments->Length();
  char **cpus = new char*[count + 1];
  cpus[count] = NULL;
  for (int i = 0; i < count; ++i) {
    cpus[i] = strdup(*Nan::Utf8String(cpusArguments->Get(Nan::New(i))->ToString()));
  }

  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Hypervisor *hypervisor = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  Nan::AsyncQueueWorker(new GetBaselineCPUWorker(callback, hypervisor->handle_, cpus, count, flags));
  return;
}

NLV_WORKER_EXECUTE(Hypervisor, GetBaselineCPU)
{
  NLV_WORKER_ASSERT_CONNECTION();
  const char *result = virConnectBaselineCPU(Handle(), (const char**)cpus_, count_, flags_);
  for (int i = 0; i < count_; ++i) {
    free(cpus_[i]);
  }

  delete [] cpus_;

  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NAN_METHOD(Hypervisor::CompareCPU)
{
  Nan::HandleScope scope;

  if (info.Length() == 0 ||
      (info.Length() < 2 && (!info[0]->IsString() && !info[1]->IsFunction()))) {
    Nan::ThrowTypeError("You must specify a cpu description and a callback");
    return;
  }

  int flags = 0;
  std::string cpu(*Nan::Utf8String(info[0]->ToString()));
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Hypervisor *hypervisor = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  Nan::AsyncQueueWorker(new CompareCPUWorker(callback, hypervisor->handle_, cpu, flags));
  return;
}

NLV_WORKER_EXECUTE(Hypervisor, CompareCPU)
{
  NLV_WORKER_ASSERT_CONNECTION();
  int result = virConnectCompareCPU(Handle(), (const char *)cpu_.c_str(), flags_);
  if (result == VIR_CPU_COMPARE_ERROR) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

#define HYPERVISOR_STRING_LIST_RETURN_EXECUTE(WorkerName, CountMethod, ListMethod)  \
  void Hypervisor::WorkerName##Worker::Execute() {  \
    NLV_WORKER_ASSERT_CONNECTION()  \
    int count = CountMethod(Handle());  \
    if (count == -1) {  \
      SetVirError(virGetLastError()); \
      return; \
    } \
    char **names = new char*[count]; \
    if (names == NULL) {  \
      SetErrorMessage("could not allocate memory"); \
      return; \
    } \
    int nameCount = ListMethod(Handle(), names, count); \
    if (nameCount == -1) {  \
      SetVirError(virGetLastError()); \
      delete [] names; \
      return; \
    } \
    for (int i = 0; i < nameCount; ++i) { \
      data_.push_back(std::string(*(names + i))); \
      free(*(names + i)); \
    } \
    delete [] names;  \
  }

#define HYPERVISOR_INT_LIST_RETURN_EXECUTE(WorkerName, CountMethod, ListMethod)  \
  void Hypervisor::WorkerName##Worker::Execute() {  \
    NLV_WORKER_ASSERT_CONNECTION()  \
    int count = CountMethod(Handle());  \
    if (count == -1) {  \
      SetVirError(virGetLastError()); \
      return; \
    } \
    int *elements = new int[count]; \
    if (elements == NULL) {  \
      SetErrorMessage("could not allocate memory"); \
      delete [] elements; \
      return; \
    } \
    int elementCount = ListMethod(Handle(), elements, count); \
    if (elementCount == -1) {  \
      SetVirError(virGetLastError()); \
      return; \
    } \
    for (int i = 0; i < elementCount; ++i) { \
      data_.push_back(*(elements + i)); \
    } \
    delete [] elements; \
  }

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListDefinedDomains)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListDefinedDomains,
  virConnectNumOfDefinedDomains, virConnectListDefinedDomains)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListDefinedNetworks)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListDefinedNetworks,
  virConnectNumOfDefinedNetworks, virConnectListDefinedNetworks)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListDefinedStoragePools)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListDefinedStoragePools,
  virConnectNumOfDefinedStoragePools, virConnectListDefinedStoragePools)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListDefinedInterfaces)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListDefinedInterfaces,
  virConnectNumOfDefinedInterfaces, virConnectListDefinedInterfaces)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListActiveDomains)
HYPERVISOR_INT_LIST_RETURN_EXECUTE(ListActiveDomains,
  virConnectNumOfDomains, virConnectListDomains)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListActiveInterfaces)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListActiveInterfaces,
  virConnectNumOfInterfaces, virConnectListInterfaces)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListNetworkFilters)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListNetworkFilters,
  virConnectNumOfNWFilters, virConnectListNWFilters)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListActiveNetworks)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListActiveNetworks,
  virConnectNumOfNetworks, virConnectListNetworks)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListSecrets)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListSecrets,
  virConnectNumOfSecrets, virConnectListSecrets)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, ListActiveStoragePools)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListActiveStoragePools,
  virConnectNumOfStoragePools, virConnectListStoragePools)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfDefinedDomains)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfDefinedDomains, virConnectNumOfDefinedDomains)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfDefinedInterfaces)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfDefinedInterfaces, virConnectNumOfDefinedInterfaces)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfDefinedNetworks)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfDefinedNetworks, virConnectNumOfDefinedNetworks)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfDefinedStoragePools)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfDefinedStoragePools, virConnectNumOfDefinedStoragePools)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfActiveDomains)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfActiveDomains, virConnectNumOfDomains)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfActiveInterfaces)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfActiveInterfaces, virConnectNumOfInterfaces)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfActiveNetworks)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfActiveNetworks, virConnectNumOfNetworks)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfActiveStoragePools)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfActiveStoragePools, virConnectNumOfStoragePools)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfNetworkFilters)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfNetworkFilters, virConnectNumOfNWFilters)

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNumberOfSecrets)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfSecrets, virConnectNumOfSecrets)

NAN_METHOD(Hypervisor::ListNodeDevices)
{
  Nan::HandleScope scope;

  if (info.Length() < 1 ||
      (info.Length() == 1 && info[0]->IsString())) {
    Nan::ThrowTypeError("You must specify a capability and/or a callback");
    return;
  }

  Nan::Callback *callback;
  std::string capability;
  if (info.Length() == 2) {
    capability = *Nan::Utf8String(info[0]);
    callback = new Nan::Callback(info[1].As<Function>());
  } else {
    callback = new Nan::Callback(info[0].As<Function>());
  }

  Hypervisor *hypervisor = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  Nan::AsyncQueueWorker(new ListNodeDevicesWorker(callback, hypervisor->handle_, capability));
  return;
}

NLV_WORKER_EXECUTE(Hypervisor, ListNodeDevices)
{
  NLV_WORKER_ASSERT_CONNECTION();
  unsigned int flags = 0;
  int num_devices =
    virNodeNumOfDevices(Handle(), (const char *) capability_.c_str(), flags);

  if (num_devices == -1) {
    SetVirError(virGetLastError());
    return;
  }

  char **names = new char*[num_devices];
  if (names == NULL) {
    SetErrorMessage("unable to allocate memory");
    return;
  }

  num_devices =
    virNodeListDevices(Handle(), (const char *)capability_.c_str(), names, num_devices, flags);
  if (num_devices == -1) {
    free(names);
    SetVirError(virGetLastError());
    return;
  }

  for (int i = 0; i < num_devices; ++i) {
    data_.push_back(names[num_devices]);
    free(names[num_devices]);
  }

  delete [] names;
}

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNodeSecurityModel)
NLV_WORKER_EXECUTE(Hypervisor, GetNodeSecurityModel)
{
  NLV_WORKER_ASSERT_CONNECTION();
  int result = virNodeGetSecurityModel(Handle(), &securityModel_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Hypervisor, GetNodeSecurityModel)
{
  Nan::HandleScope scope;

  Local<Object> result = Nan::New<Object>();
  result->Set(Nan::New("model").ToLocalChecked(), Nan::New(securityModel_.model).ToLocalChecked());
  result->Set(Nan::New("doi").ToLocalChecked(), Nan::New(securityModel_.doi).ToLocalChecked());

  v8::Local<v8::Value> argv[] = {
    Nan::Null(),
    result
  };

  callback->Call(2, argv);
}

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNodeInfo)
NLV_WORKER_EXECUTE(Hypervisor, GetNodeInfo)
{
  NLV_WORKER_ASSERT_CONNECTION();
  int result = virNodeGetInfo(Handle(), &info_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Hypervisor, GetNodeInfo)
{
  Nan::HandleScope scope;

  Local<Object> result = Nan::New<Object>();
  result->Set(Nan::New("model").ToLocalChecked(), Nan::New(info_.model).ToLocalChecked());
  result->Set(Nan::New("memory").ToLocalChecked(), Nan::New<Number>(info_.memory));
  result->Set(Nan::New("cpus").ToLocalChecked(), Nan::New<Integer>(info_.cpus));
  result->Set(Nan::New("mhz").ToLocalChecked(), Nan::New<Integer>(info_.mhz));
  result->Set(Nan::New("nodes").ToLocalChecked(), Nan::New<Integer>(info_.nodes));
  result->Set(Nan::New("sockets").ToLocalChecked(), Nan::New<Integer>(info_.sockets));
  result->Set(Nan::New("cores").ToLocalChecked(), Nan::New<Integer>(info_.cores));
  result->Set(Nan::New("threads").ToLocalChecked(), Nan::New<Integer>(info_.threads));

  v8::Local<v8::Value> argv[] = { Nan::Null(), result };
  callback->Call(2, argv);
}

NLV_WORKER_METHOD_NO_ARGS(Hypervisor, GetNodeFreeMemory)
NLV_WORKER_EXECUTE(Hypervisor, GetNodeFreeMemory)
{
  NLV_WORKER_ASSERT_CONNECTION();
  unsigned long long result = virNodeGetFreeMemory(Handle());
  if (result == 0) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NAN_METHOD(Hypervisor::GetNodeMemoryStats)
{
  Nan::HandleScope scope;
  if (info.Length() < 3 ||
      (!info[0]->IsNumber() && !info[1]->IsNumber() && !info[2]->IsFunction())) {
    Nan::ThrowTypeError("signature is numCell, flags, callback");
    return;
  }
  Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());
  Hypervisor *hypervisor = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  int numCells = info[0]->IntegerValue();
  int flags = info[1]->IntegerValue();
  Nan::AsyncQueueWorker(new GetNodeMemoryStatsWorker(callback, hypervisor->handle_, numCells, flags));
  return;
}

NLV_WORKER_EXECUTE(Hypervisor, GetNodeMemoryStats)
{
  NLV_WORKER_ASSERT_CONNECTION();
  int nparams = 0;
  int result = virNodeGetMemoryStats(Handle(), cellNum_, NULL, &nparams, flags_);
  if ( result == 0 && nparams != 0) {
      info_.resize(nparams);
      result = virNodeGetMemoryStats(Handle(), cellNum_, &info_[0], &nparams, flags_);
  }
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Hypervisor, GetNodeMemoryStats)
{
  Nan::HandleScope scope;

  Local<Object> result = Nan::New<Object>();
  for(unsigned int i = 0;i < info_.size();++i) {
    result->Set(Nan::New(info_[i].field).ToLocalChecked(), Nan::New<Number>(info_[i].value));
  }
  v8::Local<v8::Value> argv[] = { Nan::Null(), result };
  callback->Call(2, argv);
}

NAN_METHOD(Hypervisor::GetNodeCellsFreeMemory)
{
  if (info.Length() < 3 ||
      (!info[0]->IsNumber() && !info[1]->IsNumber() && !info[2]->IsFunction())) {
    Nan::ThrowTypeError("signature is startCell, maxCells, callback");
    return;
  }

  int startCell = info[0]->IntegerValue();
  int maxCells = info[1]->IntegerValue();
  if ((startCell < 0) || (maxCells <= 0) || (startCell + maxCells > 10000)) {
    Nan::ThrowTypeError("invalid cell bounds");
    return;
  }

  Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());
  Hypervisor *hypervisor = Nan::ObjectWrap::Unwrap<Hypervisor>(info.This());
  Nan::AsyncQueueWorker(new GetNodeCellsFreeMemoryWorker(callback, hypervisor->handle_, startCell, maxCells));
  return;
}

NLV_WORKER_EXECUTE(Hypervisor, GetNodeCellsFreeMemory)
{
  NLV_WORKER_ASSERT_CONNECTION();

  unsigned long long *results = NULL;
  results = (unsigned long long*) malloc(maxCells_ * sizeof(*results));
  if (results == NULL) {
    SetErrorMessage("Unable to allocate memory");
    return;
  }

  int cells =
    virNodeGetCellsFreeMemory(Handle(), results, startCell_, maxCells_);
  if (cells == -1) {
    free(results);
    SetVirError(virGetLastError());
    return;
  }

  for (int i = 0; i < cells; ++i) {
    data_.push_back(results[i]);
  }

  free(results);
}

} //namespace NLV
