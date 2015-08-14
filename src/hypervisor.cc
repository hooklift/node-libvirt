// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

#include <assert.h>

#include "worker_macros.h"

#include "domain.h"
#include "node_device.h"
#include "secret.h"
#include "storage_pool.h"
#include "storage_volume.h"
#include "network.h"
#include "network_filter.h"
#include "interface.h"
#include "hypervisor.h"

namespace NodeLibvirt {

Persistent<Function> Hypervisor::constructor;
Persistent<FunctionTemplate> Hypervisor::constructor_template;
void Hypervisor::Initialize(Handle<Object> exports)
{
  NanScope();

  Local<FunctionTemplate> t = NanNew<FunctionTemplate>(New);
  t->SetClassName(NanNew("Hypervisor"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  // methods
  NODE_SET_PROTOTYPE_METHOD(t, "connect",                 Connect);
  NODE_SET_PROTOTYPE_METHOD(t, "disconnect",              Disconnect);

  NODE_SET_PROTOTYPE_METHOD(t, "getCapabilities",         GetCapabilities);
  NODE_SET_PROTOTYPE_METHOD(t, "getHostname",             GetHostname);
  NODE_SET_PROTOTYPE_METHOD(t, "getSysInfo",              GetSysInfo);
  NODE_SET_PROTOTYPE_METHOD(t, "getType",                 GetType);
  NODE_SET_PROTOTYPE_METHOD(t, "getConnectionUri",        GetConnectionUri);
  NODE_SET_PROTOTYPE_METHOD(t, "getVersion",              GetVersion);
  NODE_SET_PROTOTYPE_METHOD(t, "getLibVirtVersion",       GetLibVirtVersion);
  NODE_SET_PROTOTYPE_METHOD(t, "isConnectionEncrypted",   IsConnectionEncrypted);
  NODE_SET_PROTOTYPE_METHOD(t, "isConnectionSecure",      IsConnectionSecure);
  NODE_SET_PROTOTYPE_METHOD(t, "isConnectionAlive",       IsConnectionAlive);
  NODE_SET_PROTOTYPE_METHOD(t, "getMaxVcpus",             GetMaxVcpus);
  NODE_SET_PROTOTYPE_METHOD(t, "setKeepAlive",            SetKeepAlive);
  NODE_SET_PROTOTYPE_METHOD(t, "getBaselineCPU",          GetBaselineCPU);
  NODE_SET_PROTOTYPE_METHOD(t, "compareCPU",              CompareCPU);

  NODE_SET_PROTOTYPE_METHOD(t, "listDefinedDomains",      ListDefinedDomains);
  NODE_SET_PROTOTYPE_METHOD(t, "listDefinedNetworks",     ListDefinedNetworks);
  NODE_SET_PROTOTYPE_METHOD(t, "listDefinedStoragePools", ListDefinedStoragePools);
  NODE_SET_PROTOTYPE_METHOD(t, "listDefinedInterfaces",   ListDefinedInterfaces);
  NODE_SET_PROTOTYPE_METHOD(t, "listActiveDomains",       ListActiveDomains);
  NODE_SET_PROTOTYPE_METHOD(t, "listActiveInterfaces",    ListActiveInterfaces);
  NODE_SET_PROTOTYPE_METHOD(t, "listActiveNetworks",      ListActiveNetworks);
  NODE_SET_PROTOTYPE_METHOD(t, "listActiveStoragePools",  ListActiveStoragePools);
  NODE_SET_PROTOTYPE_METHOD(t, "listNetworkFilters",      ListNetworkFilters);
  NODE_SET_PROTOTYPE_METHOD(t, "listSecrets",             ListSecrets);

  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfDefinedDomains", GetNumberOfDefinedDomains);
  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfDefinedInterfaces", GetNumberOfDefinedInterfaces);
  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfDefinedNetworks", GetNumberOfDefinedNetworks);
  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfDefinedStoragePools", GetNumberOfDefinedStoragePools);
  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfActiveDomains", GetNumberOfActiveDomains);
  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfActiveInterfaces", GetNumberOfActiveInterfaces);
  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfActiveNetworks", GetNumberOfActiveNetworks);
  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfNetworkFilters", GetNumberOfNetworkFilters);
  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfSecrets", GetNumberOfSecrets);
  NODE_SET_PROTOTYPE_METHOD(t, "getNumberOfActiveStoragePools", GetNumberOfActiveStoragePools);

  // NODE
  NODE_SET_PROTOTYPE_METHOD(t, "listNodeDevices",             ListNodeDevices);
  NODE_SET_PROTOTYPE_METHOD(t, "getNodeSecurityModel",        GetNodeSecurityModel);
  NODE_SET_PROTOTYPE_METHOD(t, "getNodeInfo",                 GetNodeInfo);
  NODE_SET_PROTOTYPE_METHOD(t, "getNodeFreeMemory",           GetNodeFreeMemory);
  NODE_SET_PROTOTYPE_METHOD(t, "getNodeMemoryStats",          GetNodeMemoryStats);
  NODE_SET_PROTOTYPE_METHOD(t, "getNodeCellsFreeMemory",      GetNodeCellsFreeMemory);

  // INTERFACE
  NODE_SET_PROTOTYPE_METHOD(t, "lookupInterfaceByName",       Interface::LookupByName);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupInterfaceByMacAddress", Interface::LookupByMacAddress);
  NODE_SET_PROTOTYPE_METHOD(t, "defineInterface",             Interface::Define);

  // NETWORK
  NODE_SET_PROTOTYPE_METHOD(t, "createNetwork",               Network::Create);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkByName",         Network::LookupByName);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkByUUID",         Network::LookupByUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "defineNetwork",               Network::Define);

  // NETWORK FILTER
  NODE_SET_PROTOTYPE_METHOD(t, "defineNetworkFilter",         NetworkFilter::Define);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkFilterByName",   NetworkFilter::LookupByName);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkFilterByUUID",   NetworkFilter::LookupByUUID);

  // STORAGE POOL
  NODE_SET_PROTOTYPE_METHOD(t, "createStoragePool",           StoragePool::Create);
  NODE_SET_PROTOTYPE_METHOD(t, "defineStoragePool",           StoragePool::Define);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupStoragePoolByName",     StoragePool::LookupByName);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupStoragePoolByUUID",     StoragePool::LookupByUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupStoragePoolByVolume",   StoragePool::LookupByVolume);

  // STORAGE VOLUME
  NODE_SET_PROTOTYPE_METHOD(t, "lookupStorageVolumeByKey",    StorageVolume::LookupByKey);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupStorageVolumeByPath",   StorageVolume::LookupByPath);

  // SECRET
  NODE_SET_PROTOTYPE_METHOD(t, "defineSecret",                Secret::Define);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupSecretByUsage",         Secret::LookupByUsage);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupSecretByUUID",          Secret::LookupByUUID);

  // NODE
  NODE_SET_PROTOTYPE_METHOD(t, "lookupNodeDeviceByName",      NodeDevice::LookupByName);
  NODE_SET_PROTOTYPE_METHOD(t, "createNodeDevice",            NodeDevice::Create);

  // DOMAIN
  NODE_SET_PROTOTYPE_METHOD(t, "createDomain",                Domain::Create);
  NODE_SET_PROTOTYPE_METHOD(t, "defineDomain",                Domain::Define);
  NODE_SET_PROTOTYPE_METHOD(t, "restoreDomain",               Domain::Restore);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupDomainById",            Domain::LookupById);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupDomainByName",          Domain::LookupByName);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupDomainByUUID",          Domain::LookupByUUID);

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

  t->SetClassName(NanNew("Hypervisor"));
  NanAssignPersistent(constructor_template, t);
  NanAssignPersistent(constructor, t->GetFunction());
  exports->Set(NanNew("Hypervisor"), t->GetFunction());
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
  NanScope();

  if (!args.IsConstructCall()) {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = NanNew<Function>(constructor);
    NanReturnValue(cons->NewInstance(argc, argv));
  }

  int argsLen = args.Length();
  if (argsLen == 0) {
    NanThrowTypeError("You must specify a connection URI as first argument");
    NanReturnUndefined();
  }

  if (argsLen == 1 && !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as connection URI");
    NanReturnUndefined();
  }

  bool readOnly = false;
  std::string username;
  std::string password;
  std::string uri = *NanUtf8String(args[0]);

  if (argsLen >= 2) {
    if (args[1]->IsBoolean()) {
      readOnly = args[1]->IsTrue();
    } else if (args[1]->IsObject()) {
      Local<Object> options = args[1].As<Object>();
      readOnly = NanBooleanOptionValue(options, NanNew("readOnly"), false);

      if (options->Has(NanNew("username")))
        username = *NanUtf8String(options->Get(NanNew("username")));

      if (options->Has(NanNew("password")))
        password = *NanUtf8String(options->Get(NanNew("password")));
    }
  }

  Hypervisor *hypervisor = new Hypervisor(uri, username, password, readOnly);
  hypervisor->Wrap(args.This());

  // add values to instance object
  args.This()->Set(NanNew("uri"), NanNew(uri));
  args.This()->Set(NanNew("username"), NanNew(username));
  args.This()->Set(NanNew("password"), NanNew(password));
  args.This()->Set(NanNew("readOnly"), NanNew(readOnly));

  NanReturnValue(args.This());
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
  NanScope();
  if (args.Length() == 1 && !args[0]->IsFunction()) {
    NanThrowTypeError("You must specify a function as first argument");
    NanReturnUndefined();
  }

  NanCallback *callback = new NanCallback(args[0].As<Function>());
  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  NanAsyncQueueWorker(new ConnectWorker(callback, hv));
  NanReturnUndefined();
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
  NanScope();
  if (args.Length() == 1 && !args[0]->IsFunction()) {
    NanThrowTypeError("You must specify a function as first argument");
    NanReturnUndefined();
  }

  NanCallback *callback = new NanCallback(args[0].As<Function>());
  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  NanAsyncQueueWorker(new DisconnectWorker(callback, hv));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Hypervisor, Disconnect)
{
  NLV_WORKER_ASSERT_CONNECTION();
  int result = virConnectClose(Handle());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

//  assert(result == 0);
  hypervisor_->handle_ = NULL;
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
  NanScope();

  if (args.Length() < 2) {
    NanThrowTypeError("You must specify a type and callback");
    NanReturnUndefined();
  }

  if (!args[0]->IsString()) {
    NanThrowTypeError("Type must be a string");
    NanReturnUndefined();
  }

  if (!args[1]->IsFunction()) {
    NanThrowTypeError("Second argument must be a callback");
    NanReturnUndefined();
  }

  std::string type(*NanAsciiString(args[0]));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
  NanAsyncQueueWorker(new GetMaxVcpusWorker(callback, hypervisor->handle_, type));
  NanReturnUndefined();
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
  NanScope();

  if (args.Length() < 3) {
    NanThrowTypeError("You must specify an interval, count and callback");
    NanReturnUndefined();
  }

  if (!args[0]->IsNumber()) {
    NanThrowTypeError("interval must be a number");
    NanReturnUndefined();
  }

  if (!args[1]->IsNumber()) {
    NanThrowTypeError("count must be a number");
    NanReturnUndefined();
  }

  if (!args[2]->IsFunction()) {
    NanThrowTypeError("third argument must be callback");
    NanReturnUndefined();
  }

  std::string type(*NanAsciiString(args[0]));
  int interval = args[0]->IntegerValue();
  unsigned int count = args[1]->IntegerValue();
  NanCallback *callback = new NanCallback(args[2].As<Function>());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
  NanAsyncQueueWorker(new SetKeepAliveWorker(callback, hypervisor->handle_, interval, count));
  NanReturnUndefined();
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
  NanScope();

  if (args.Length() == 0 ||
      (args.Length() < 2 && (!args[0]->IsArray() && !args[1]->IsFunction()))) {
    NanThrowTypeError("You must specify an array with two cpu descriptions and a callback");
    NanReturnUndefined();
  }

  Local<Array> cpusArguments = args[0].As<Array>();

  int flags = 0;
  int count = cpusArguments->Length();
  char **cpus = new char*[count + 1];
  cpus[count] = NULL;
  for (int i = 0; i < count; ++i) {
    cpus[i] = strdup(*NanUtf8String(cpusArguments->Get(NanNew(i))->ToString()));
  }

  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
  NanAsyncQueueWorker(new GetBaselineCPUWorker(callback, hypervisor->handle_, cpus, count, flags));
  NanReturnUndefined();
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
  NanScope();

  if (args.Length() == 0 ||
      (args.Length() < 2 && (!args[0]->IsString() && !args[1]->IsFunction()))) {
    NanThrowTypeError("You must specify a cpu description and a callback");
    NanReturnUndefined();
  }

  int flags = 0;
  std::string cpu(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
  NanAsyncQueueWorker(new CompareCPUWorker(callback, hypervisor->handle_, cpu, flags));
  NanReturnUndefined();
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
    char **names = (char **)malloc(sizeof(*names) * count); \
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
    int *elements = (int *)malloc(sizeof(*elements) * count); \
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
  NanScope();

  if (args.Length() < 1 ||
      (args.Length() == 1 && args[0]->IsString())) {
    NanThrowTypeError("You must specify a capability and/or a callback");
    NanReturnUndefined();
  }

  NanCallback *callback;
  std::string capability;
  if (args.Length() == 2) {
    capability = *NanAsciiString(args[0]);
    callback = new NanCallback(args[1].As<Function>());
  } else {
    callback = new NanCallback(args[0].As<Function>());
  }

  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
  NanAsyncQueueWorker(new ListNodeDevicesWorker(callback, hypervisor->handle_, capability));
  NanReturnUndefined();
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

  char **names = (char **) malloc(sizeof(*names) * num_devices);
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
  NanScope();

  Local<Object> result = NanNew<Object>();
  result->Set(NanNew("model"), NanNew(securityModel_.model));
  result->Set(NanNew("doi"), NanNew(securityModel_.doi));

  v8::Local<v8::Value> argv[] = {
    NanNull(),
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
  NanScope();

  Local<Object> result = NanNew<Object>();
  result->Set(NanNew("model"), NanNew(info_.model));
  result->Set(NanNew("memory"), NanNew<Number>(info_.memory));
  result->Set(NanNew("cpus"), NanNew<Integer>(info_.cpus));
  result->Set(NanNew("mhz"), NanNew<Integer>(info_.mhz));
  result->Set(NanNew("nodes"), NanNew<Integer>(info_.nodes));
  result->Set(NanNew("sockets"), NanNew<Integer>(info_.sockets));
  result->Set(NanNew("cores"), NanNew<Integer>(info_.cores));
  result->Set(NanNew("threads"), NanNew<Integer>(info_.threads));

  v8::Local<v8::Value> argv[] = { NanNull(), result };
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
  NanScope();
  if (args.Length() < 3 ||
      (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsFunction())) {
    NanThrowTypeError("signature is numCell, flags, callback");
    NanReturnUndefined();
  }
  NanCallback *callback = new NanCallback(args[2].As<Function>());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
  int numCells = args[0]->IntegerValue();
  int flags = args[1]->IntegerValue();
  NanAsyncQueueWorker(new GetNodeMemoryStatsWorker(callback, hypervisor->handle_, numCells, flags));
  NanReturnUndefined();
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
  NanScope();

  Local<Object> result = NanNew<Object>();
  for(unsigned int i = 0;i < info_.size();++i) {
    result->Set(NanNew(info_[i].field), NanNew<Number>(info_[i].value));
  }
  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NAN_METHOD(Hypervisor::GetNodeCellsFreeMemory)
{
  if (args.Length() < 3 ||
      (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsFunction())) {
    NanThrowTypeError("signature is startCell, maxCells, callback");
    NanReturnUndefined();
  }

  int startCell = args[0]->IntegerValue();
  int maxCells = args[1]->IntegerValue();
  if ((startCell < 0) || (maxCells <= 0) || (startCell + maxCells > 10000)) {
    NanThrowTypeError("invalid cell bounds");
    NanReturnUndefined();
  }

  NanCallback *callback = new NanCallback(args[2].As<Function>());
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
  NanAsyncQueueWorker(new GetNodeCellsFreeMemoryWorker(callback, hypervisor->handle_, startCell, maxCells));
  NanReturnUndefined();
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

} //namespace NodeLibvirt
