// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

// #include "event_impl.h"
// #include "domain.h"
// #include "node_device.h"
// #include "network_filter.h"
// #include "error.h"

#include "secret.h"
#include "storage_pool.h"
#include "storage_volume.h"
#include "network.h"
#include "interface.h"
#include "hypervisor.h"

#define HYPERVISOR_NOARGS_WORKER_METHOD(name) \
NAN_METHOD(Hypervisor::name) {  \
  NanScope(); \
  if (args.Length() == 1 && !args[0]->IsFunction()) { \
    NanThrowTypeError("You must specify a function as first argument"); \
    NanReturnUndefined(); \
  } \
  NanCallback *callback = new NanCallback(args[0].As<Function>());  \
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This()); \
  NanAsyncQueueWorker(new name##Worker(callback, hypervisor));  \
  NanReturnUndefined(); \
}

#define HYPERVISOR_ASSERT_CONNECTION() \
  if (Connection() == NULL) { \
    SetErrorMessage("invalid connection");  \
    return; \
  }

namespace NodeLibvirt {

// static Persistent<String> domain_event_callback_symbol;
// static Persistent<String> domain_event_type_symbol;
// static Persistent<String> domain_event_symbol;
// static Persistent<String> domain_event_detail_symbol;
// static Persistent<String> domain_event_hypervisor_symbol;
// static Persistent<String> domain_event_rtc_utcoffset_symbol;
// static Persistent<String> domain_event_action_symbol;
// static Persistent<String> domain_event_ioerror_srcpath_symbol;
// static Persistent<String> domain_event_ioerror_devalias_symbol;
// static Persistent<String> domain_event_ioerror_reason_symbol;
// static Persistent<String> domain_event_graphics_family_sym;
// static Persistent<String> domain_event_graphics_node_sym;
// static Persistent<String> domain_event_graphics_service_sym;
// static Persistent<String> domain_event_graphics_subjname_sym;
// static Persistent<String> domain_event_graphics_subjtype_sym;
// static Persistent<String> domain_event_graphics_local_sym;
// static Persistent<String> domain_event_graphics_remote_sym;
// static Persistent<String> domain_event_graphics_subject_sym;
// static Persistent<String> domain_event_graphics_phase_sym;
// static Persistent<String> domain_event_graphics_authscheme_sym;

Persistent<Function> Hypervisor::constructor;
Persistent<FunctionTemplate> Hypervisor::constructor_template;
void Hypervisor::Initialize(Handle<Object> exports)
{
  NanScope();

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
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
  NODE_SET_PROTOTYPE_METHOD(t, "listNodeDevices", ListNodeDevices);
  NODE_SET_PROTOTYPE_METHOD(t, "getNodeSecurityModel", GetNodeSecurityModel);
  NODE_SET_PROTOTYPE_METHOD(t, "getNodeInfo", GetNodeInfo);
  NODE_SET_PROTOTYPE_METHOD(t, "getNodeFreeMemory", GetNodeFreeMemory);
  NODE_SET_PROTOTYPE_METHOD(t, "getNodeCellsFreeMemory", GetNodeCellsFreeMemory);

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
  // NODE_SET_PROTOTYPE_METHOD(t, "defineNetworkFilter",         NetworkFilter::Define);
  // NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkFilterByName",   NetworkFilter::LookupByName);
  // NODE_SET_PROTOTYPE_METHOD(t, "lookupNetworkFilterByUUID",   NetworkFilter::LookupByUUID);

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


/*
  // NODE
  NODE_SET_PROTOTYPE_METHOD(t, "lookupNodeDeviceByName", NodeDevice::LookupByName);
  NODE_SET_PROTOTYPE_METHOD(t, "createNodeDevice", NodeDevice::Create);

  // DOMAIN
  NODE_SET_PROTOTYPE_METHOD(t, "registerDomainEvent", RegisterDomainEvent);
  NODE_SET_PROTOTYPE_METHOD(t, "unregisterDomainEvent", UnregisterDomainEvent);

  NODE_SET_PROTOTYPE_METHOD(t, "createDomain", Domain::Create);
  NODE_SET_PROTOTYPE_METHOD(t, "defineDomain", Domain::Define);
  NODE_SET_PROTOTYPE_METHOD(t, "restoreDomain", Domain::Restore);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupDomainById", Domain::LookupById);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupDomainByName", Domain::LookupByName);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupDomainByUUID", Domain::LookupByUUID);



  Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

  //Constants initialization
  // virConnectCredentialType
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_USERNAME);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_AUTHNAME);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_LANGUAGE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_CNONCE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_PASSPHRASE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_ECHOPROMPT);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_NOECHOPROMPT);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_REALM);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CRED_EXTERNAL);

  // virCPUCompareResult
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CPU_COMPARE_ERROR);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CPU_COMPARE_INCOMPATIBLE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CPU_COMPARE_IDENTICAL);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_CPU_COMPARE_SUPERSET);

  // virSecretUsageType
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_SECRET_USAGE_TYPE_NONE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_SECRET_USAGE_TYPE_VOLUME);

  // virDomainEventID
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_LIFECYCLE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_REBOOT);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_RTC_CHANGE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_WATCHDOG);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_IO_ERROR);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_GRAPHICS);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_IO_ERROR_REASON);
#ifdef VIR_ENUM_SENTINELS
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_ID_LAST);
#endif


  domain_event_callback_symbol         = NODE_PSYMBOL("callback");
  domain_event_type_symbol             = NODE_PSYMBOL("evtype");
  domain_event_detail_symbol           = NODE_PSYMBOL("detail");
  domain_event_symbol                  = NODE_PSYMBOL("domain");
  domain_event_hypervisor_symbol       = NODE_PSYMBOL("hypervisor");
  domain_event_rtc_utcoffset_symbol    = NODE_PSYMBOL("utc_offset");
  domain_event_action_symbol           = NODE_PSYMBOL("action");
  domain_event_ioerror_srcpath_symbol  = NODE_PSYMBOL("src_path");
  domain_event_ioerror_devalias_symbol = NODE_PSYMBOL("dev_alias");
  domain_event_ioerror_reason_symbol   = NODE_PSYMBOL("reason");

  domain_event_graphics_family_sym  = NODE_PSYMBOL("family");
  domain_event_graphics_node_sym    = NODE_PSYMBOL("node");
  domain_event_graphics_service_sym = NODE_PSYMBOL("service");
  domain_event_graphics_subjname_sym      = NODE_PSYMBOL("name");
  domain_event_graphics_subjtype_sym      = NODE_PSYMBOL("type");

  domain_event_graphics_local_sym         = NODE_PSYMBOL("local");
  domain_event_graphics_remote_sym        = NODE_PSYMBOL("remote");
  domain_event_graphics_subject_sym       = NODE_PSYMBOL("subject");
  domain_event_graphics_phase_sym         = NODE_PSYMBOL("phase");
  domain_event_graphics_authscheme_sym    = NODE_PSYMBOL("auth_scheme");

  t->SetClassName(String::NewSymbol("Hypervisor"));
  target->Set(String::NewSymbol("Hypervisor"), t->GetFunction());
*/

  NanAssignPersistent(constructor_template, t);
  NanAssignPersistent(constructor, t->GetFunction());
  exports->Set(NanNew("Hypervisor"), t->GetFunction());
}


Hypervisor::Hypervisor(string uri, string username, string password, bool readonly)
  : ObjectWrap(),
    uri_(uri),
    username_(username),
    password_(password),
    readOnly_(readonly)
{
}

Hypervisor::~Hypervisor()
{
}

virConnectPtr Hypervisor::Connection() const
{
  return conn_;
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
  string username;
  string password;
  string uri = *NanUtf8String(args[0]);

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

HYPERVISOR_NOARGS_WORKER_METHOD(Connect)
void Hypervisor::ConnectWorker::Execute()
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

  hypervisor_->conn_ = virConnectOpenAuth((const char*) hypervisor_->uri_.c_str(), &auth,
                                          hypervisor_->readOnly_ ? VIR_CONNECT_RO : 0);
  if (hypervisor_->conn_ == NULL)
    SetVirError(virGetLastError());
}

HYPERVISOR_NOARGS_WORKER_METHOD(Disconnect)
void Hypervisor::DisconnectWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();
  if (hypervisor_->conn_ != NULL) {
    if (virConnectClose(hypervisor_->conn_) == -1)
      SetVirError(virGetLastError());
  }

  hypervisor_->conn_ = NULL;
}

#define HYPERVISOR_STRING_RETURN_EXECUTE(MethodName, Accessor)  \
  void Hypervisor::MethodName##Worker::Execute() {  \
    HYPERVISOR_ASSERT_CONNECTION(); \
    char *result = Accessor(Connection()); \
    if (result == NULL) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = result; \
    free(result); \
  }

#define HYPERVISOR_STRING_RETURN_EXECUTE_NO_FREE(MethodName, Accessor)  \
  void Hypervisor::MethodName##Worker::Execute() {  \
    HYPERVISOR_ASSERT_CONNECTION(); \
    const char *result = Accessor(Connection()); \
    if (result == NULL) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = result; \
  }


HYPERVISOR_NOARGS_WORKER_METHOD(GetCapabilities)
HYPERVISOR_STRING_RETURN_EXECUTE(GetCapabilities, virConnectGetCapabilities)

HYPERVISOR_NOARGS_WORKER_METHOD(GetHostname)
HYPERVISOR_STRING_RETURN_EXECUTE(GetHostname, virConnectGetHostname)

HYPERVISOR_NOARGS_WORKER_METHOD(GetSysInfo)
void Hypervisor::GetSysInfoWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();
  char *result = virConnectGetSysinfo(Connection(), 0);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

HYPERVISOR_NOARGS_WORKER_METHOD(GetType)
HYPERVISOR_STRING_RETURN_EXECUTE_NO_FREE(GetType, virConnectGetType)

HYPERVISOR_NOARGS_WORKER_METHOD(GetConnectionUri)
HYPERVISOR_STRING_RETURN_EXECUTE(GetConnectionUri, virConnectGetURI)

HYPERVISOR_NOARGS_WORKER_METHOD(GetVersion)
void Hypervisor::GetVersionWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();

  unsigned long version;
  int result = virConnectGetVersion(Connection(), &version);
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

HYPERVISOR_NOARGS_WORKER_METHOD(GetLibVirtVersion)
void Hypervisor::GetLibVirtVersionWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();

  unsigned long version;
  int result = virConnectGetLibVersion(Connection(), &version);
  if (result == -1) {
    SetVirError(virGetLastError());
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

#define HYPERVISOR_BOOL_RETURN_EXECUTE(MethodName, Accessor)  \
  void Hypervisor::MethodName##Worker::Execute() {  \
    HYPERVISOR_ASSERT_CONNECTION(); \
    int result = Accessor(Connection());  \
    if (result == -1) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = static_cast<bool>(result);  \
  }

#define HYPERVISOR_INT_RETURN_EXECUTE(MethodName, Accessor)  \
  void Hypervisor::MethodName##Worker::Execute() {  \
    HYPERVISOR_ASSERT_CONNECTION(); \
    int result = Accessor(Connection());  \
    if (result == -1) { \
      SetVirError(virGetLastError()); \
      return; \
    } \
    data_ = result;  \
  }

HYPERVISOR_NOARGS_WORKER_METHOD(IsConnectionEncrypted)
HYPERVISOR_BOOL_RETURN_EXECUTE(IsConnectionEncrypted, virConnectIsEncrypted)

HYPERVISOR_NOARGS_WORKER_METHOD(IsConnectionSecure)
HYPERVISOR_BOOL_RETURN_EXECUTE(IsConnectionSecure, virConnectIsSecure)

HYPERVISOR_NOARGS_WORKER_METHOD(IsConnectionAlive)
HYPERVISOR_BOOL_RETURN_EXECUTE(IsConnectionAlive, virConnectIsAlive)

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
  NanAsyncQueueWorker(new GetMaxVcpusWorker(callback, hypervisor, type));
  NanReturnUndefined();
}

void Hypervisor::GetMaxVcpusWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();
  int result = virConnectGetMaxVcpus(Connection(), type_.c_str());
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
  NanAsyncQueueWorker(new SetKeepAliveWorker(callback, hypervisor, interval, count));
  NanReturnUndefined();
}

void Hypervisor::SetKeepAliveWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();

  int result = virConnectSetKeepAlive(Connection(), interval_, count_);
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
  NanAsyncQueueWorker(new GetBaselineCPUWorker(callback, hypervisor, cpus, count, flags));
  NanReturnUndefined();
}

void Hypervisor::GetBaselineCPUWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();
  const char *result = virConnectBaselineCPU(Connection(), (const char**)cpus_, count_, flags_);
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
  NanAsyncQueueWorker(new CompareCPUWorker(callback, hypervisor, cpu, flags));
  NanReturnUndefined();
}

void Hypervisor::CompareCPUWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();
  int result = virConnectCompareCPU(Connection(), (const char *)cpu_.c_str(), flags_);
  if (result == VIR_CPU_COMPARE_ERROR) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

#define HYPERVISOR_STRING_LIST_RETURN_EXECUTE(WorkerName, CountMethod, ListMethod)  \
  void Hypervisor::WorkerName##Worker::Execute() {  \
    HYPERVISOR_ASSERT_CONNECTION()  \
    int count = CountMethod(Connection());  \
    if (count == -1) {  \
      SetVirError(virGetLastError()); \
      return; \
    } \
    char **names = (char **)malloc(sizeof(*names) * count); \
    if (names == NULL) {  \
      SetErrorMessage("could not allocate memory"); \
      return; \
    } \
    int nameCount = ListMethod(Connection(), names, count); \
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
    HYPERVISOR_ASSERT_CONNECTION()  \
    int count = CountMethod(Connection());  \
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
    int elementCount = ListMethod(Connection(), elements, count); \
    if (elementCount == -1) {  \
      SetVirError(virGetLastError()); \
      return; \
    } \
    for (int i = 0; i < elementCount; ++i) { \
      data_.push_back(*(elements + i)); \
    } \
    delete [] elements; \
  }


HYPERVISOR_NOARGS_WORKER_METHOD(ListDefinedDomains)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListDefinedDomains,
  virConnectNumOfDefinedDomains, virConnectListDefinedDomains)

HYPERVISOR_NOARGS_WORKER_METHOD(ListDefinedNetworks)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListDefinedNetworks,
  virConnectNumOfDefinedNetworks, virConnectListDefinedNetworks)

HYPERVISOR_NOARGS_WORKER_METHOD(ListDefinedStoragePools)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListDefinedStoragePools,
  virConnectNumOfDefinedStoragePools, virConnectListDefinedStoragePools)

HYPERVISOR_NOARGS_WORKER_METHOD(ListDefinedInterfaces)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListDefinedInterfaces,
  virConnectNumOfDefinedInterfaces, virConnectListDefinedInterfaces)

HYPERVISOR_NOARGS_WORKER_METHOD(ListActiveDomains)
HYPERVISOR_INT_LIST_RETURN_EXECUTE(ListActiveDomains,
  virConnectNumOfDomains, virConnectListDomains)

HYPERVISOR_NOARGS_WORKER_METHOD(ListActiveInterfaces)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListActiveInterfaces,
  virConnectNumOfInterfaces, virConnectListInterfaces)

HYPERVISOR_NOARGS_WORKER_METHOD(ListNetworkFilters)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListNetworkFilters,
  virConnectNumOfNWFilters, virConnectListNWFilters)

HYPERVISOR_NOARGS_WORKER_METHOD(ListActiveNetworks)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListActiveNetworks,
  virConnectNumOfNetworks, virConnectListNetworks)

HYPERVISOR_NOARGS_WORKER_METHOD(ListSecrets)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListSecrets,
  virConnectNumOfSecrets, virConnectListSecrets)

HYPERVISOR_NOARGS_WORKER_METHOD(ListActiveStoragePools)
HYPERVISOR_STRING_LIST_RETURN_EXECUTE(ListActiveStoragePools,
  virConnectNumOfStoragePools, virConnectListStoragePools)


HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfDefinedDomains)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfDefinedDomains, virConnectNumOfDefinedDomains)

HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfDefinedInterfaces)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfDefinedInterfaces, virConnectNumOfDefinedInterfaces)

HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfDefinedNetworks)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfDefinedNetworks, virConnectNumOfDefinedNetworks)

HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfDefinedStoragePools)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfDefinedStoragePools, virConnectNumOfDefinedStoragePools)

HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfActiveDomains)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfActiveDomains, virConnectNumOfDomains)

HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfActiveInterfaces)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfActiveInterfaces, virConnectNumOfInterfaces)

HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfActiveNetworks)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfActiveNetworks, virConnectNumOfNetworks)

HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfActiveStoragePools)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfActiveStoragePools, virConnectNumOfStoragePools)

HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfNetworkFilters)
HYPERVISOR_INT_RETURN_EXECUTE(GetNumberOfNetworkFilters, virConnectNumOfNWFilters)

HYPERVISOR_NOARGS_WORKER_METHOD(GetNumberOfSecrets)
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
  NanAsyncQueueWorker(new ListNodeDevicesWorker(callback, hypervisor, capability));
  NanReturnUndefined();
}

void Hypervisor::ListNodeDevicesWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();
  unsigned int flags = 0;
  int num_devices =
    virNodeNumOfDevices(Connection(), (const char *) capability_.c_str(), flags);

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
    virNodeListDevices(Connection(), (const char *)capability_.c_str(), names, num_devices, flags);
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

HYPERVISOR_NOARGS_WORKER_METHOD(GetNodeSecurityModel)
void Hypervisor::GetNodeSecurityModelWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();
  int result = virNodeGetSecurityModel(Connection(), &securityModel_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

void Hypervisor::GetNodeSecurityModelWorker::HandleOKCallback()
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

HYPERVISOR_NOARGS_WORKER_METHOD(GetNodeInfo)
void Hypervisor::GetNodeInfoWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();
  int result = virNodeGetInfo(Connection(), &info_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

void Hypervisor::GetNodeInfoWorker::HandleOKCallback()
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

HYPERVISOR_NOARGS_WORKER_METHOD(GetNodeFreeMemory)
void Hypervisor::GetNodeFreeMemoryWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();
  unsigned long long result = virNodeGetFreeMemory(Connection());
  if (result == 0) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
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
  NanAsyncQueueWorker(new GetNodeCellsFreeMemoryWorker(callback, hypervisor, startCell, maxCells));
  NanReturnUndefined();
}

void Hypervisor::GetNodeCellsFreeMemoryWorker::Execute()
{
  HYPERVISOR_ASSERT_CONNECTION();

  unsigned long long *results = NULL;
  results = (unsigned long long*) malloc(maxCells_ * sizeof(*results));
  if (results == NULL) {
    SetErrorMessage("Unable to allocate memory");
    return;
  }

  int cells =
    virNodeGetCellsFreeMemory(Connection(), results, startCell_, maxCells_);
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



/*
Handle<Value> Hypervisor::GetNodeInfo(const Arguments& args) {
    HandleScope scope;
    virNodeInfo info_;
    int ret = -1;

    Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
    ret = virNodeGetInfo(hypervisor->conn_, &info_);

    if(ret == -1) {
        ThrowException(Error::New(virGetLastError()));
        return Null();
    }

    Local<Object> info = Object::New();
    info->Set(node_info_model_symbol, String::New(info_.model));
    info->Set(node_info_memory_symbol, Number::New(info_.memory));
    info->Set(node_info_cpus_symbol, Integer::New(info_.cpus));
    info->Set(node_info_mhz_symbol, Integer::New(info_.mhz));
    info->Set(node_info_nodes_symbol, Integer::New(info_.nodes));
    info->Set(node_info_sockets_symbol, Integer::New(info_.sockets));
    info->Set(node_info_cores_symbol, Integer::New(info_.cores));
    info->Set(node_info_threads_symbol, Integer::New(info_.threads));

    return scope.Close(info);
}
*/


/*
  Handle<Value> Hypervisor::GetNodeSecurityModel(const Arguments& args) {
      HandleScope scope;
      virSecurityModel secmodel;
      int ret = -1;

      Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
      ret = virNodeGetSecurityModel(hypervisor->conn_, &secmodel);

      if(ret == -1) {
          ThrowException(Error::New(virGetLastError()));
          return Null();
      }

      Local<Object> object = Object::New();
      object->Set(security_model_symbol, String::New(secmodel.model));
      object->Set(security_model_doi_symbol, String::New(secmodel.doi));

      return scope.Close(object);
  }
*/







/*

  Handle<Value> Hypervisor::SetKeepAlive(const Arguments& args) {
      HandleScope scope;
      int ret = -1;

      if (args.Length() != 2) {
          return ThrowException(Exception::TypeError(
          String::New("You must specify two integer arguments interval and count")));
      }

      if (!args[0]->IsNumber()) {
          return ThrowException(Exception::TypeError(
          String::New("Interval must be a number")));
      }

      if (!args[1]->IsNumber()) {
          return ThrowException(Exception::TypeError(
          String::New("Count must be a number")));
      }

      Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

      ret = virConnectSetKeepAlive(hypervisor->conn_, args[0]->Int32Value(), args[1]->Int32Value());

      if (ret == -1) {
          return ThrowException(Error::New(virGetLastError()));
      }

      if (ret == 1) {
          return ThrowException(Exception::TypeError(
          String::New("Remote party doesn't support keepalive messages")));
      }

      return scope.Close(Undefined());
  }



  Handle<Value> Hypervisor::GetActiveDomains(const Arguments& args) {
      HandleScope scope;
      int *ids = NULL;
      int numOfDomains = 0;

      Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

      numOfDomains = virConnectNumOfDomains(hypervisor->conn_);

      if(numOfDomains == -1) {
          ThrowException(Error::New(virGetLastError()));
          return Null();
      }

      ids = new int[numOfDomains];
      if(ids == NULL) {
          LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
          return Null();
      }

      int ret = virConnectListDomains(hypervisor->conn_, ids, numOfDomains);

      if(ret == -1) {
          ThrowException(Error::New(virGetLastError()));
          delete [] ids;
          return Null();
      }

      Local<Array> v8Array = Array::New(numOfDomains);
      for(int i = 0; i < numOfDomains; i++) {
          v8Array->Set(Integer::New(i), Integer::New(ids[i]));
          //free(ids[i]);
      }
      delete [] ids;
      return scope.Close(v8Array);
  }

*/

} //namespace NodeLibvirt




















/*

  Handle<Value> Hypervisor::RegisterDomainEvent(const Arguments& args) {
      HandleScope scope;

      if(args.Length() == 0 || !args[0]->IsObject()) {
          return ThrowException(Exception::TypeError(
          String::New("You must specify a object as argument")));
      }

      Local<Object> arg_obj = args[0]->ToObject();

      if( !arg_obj->Has(domain_event_type_symbol) ||
          !arg_obj->Get(domain_event_type_symbol)->IsInt32()) {
          return ThrowException(Exception::TypeError(
          String::New("You must specify an valid event type")));
      }

      if( !arg_obj->Has(domain_event_callback_symbol) ||
          !arg_obj->Get(domain_event_callback_symbol)->IsFunction()) {
          return ThrowException(Exception::TypeError(
          String::New("You must specify a valid callback function")));
      }

      Domain *domain = NULL;

      if(arg_obj->Has(domain_event_symbol)) {
          Local<Object> domain_obj = arg_obj->Get(domain_event_symbol)->ToObject();
          if(!Domain::HasInstance(domain_obj)) {
              return ThrowException(Exception::TypeError(
              String::New("You must specify a Domain object instance")));
          }

          domain = ObjectWrap::Unwrap<Domain>(domain_obj);
      }

      virConnectDomainEventGenericCallback callback = NULL;
      int evtype = arg_obj->Get(domain_event_type_symbol)->Int32Value();

      switch (evtype) {
          case VIR_DOMAIN_EVENT_ID_LIFECYCLE:
              callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_lifecycle_callback);
              break;
          case VIR_DOMAIN_EVENT_ID_REBOOT:
              callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_generic_callback);
              break;
          case VIR_DOMAIN_EVENT_ID_RTC_CHANGE:
              callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_rtcchange_callback);
              break;
          case VIR_DOMAIN_EVENT_ID_WATCHDOG:
              callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_watchdog_callback);
              break;
          case VIR_DOMAIN_EVENT_ID_IO_ERROR:
              callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_io_error_callback);
              break;
          case VIR_DOMAIN_EVENT_ID_IO_ERROR_REASON:
              callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_io_error_reason_callback);
              break;
          case VIR_DOMAIN_EVENT_ID_GRAPHICS:
              callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_graphics_callback);
              break;
          default:
              callback = VIR_DOMAIN_EVENT_CALLBACK(domain_event_generic_callback);
              break;
      }

      Local<Value> jscallback = arg_obj->Get(domain_event_callback_symbol);

      Persistent<Object> opaque = Persistent<Object>::New(Object::New());
      opaque->Set(domain_event_hypervisor_symbol, args.This());
      opaque->Set(domain_event_callback_symbol, jscallback);

      Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
      int ret = virConnectDomainEventRegisterAny( hypervisor->conn_,
                                                  domain != NULL ? domain->domain() : NULL,
                                                  evtype, callback,
                                                  *opaque, domain_event_free);
      if(ret == -1) {
          ThrowException(Error::New(virGetLastError()));
          return Null();
      }

      return scope.Close(Integer::New(ret));
  }

  Handle<Value> Hypervisor::UnregisterDomainEvent(const Arguments& args) {
      HandleScope scope;
      int ret = -1;
      int callback_id = 0;

      if(args.Length() == 0 || !args[0]->IsInt32()) {
          return ThrowException(Exception::TypeError(
          String::New("You must specify a integer as argument to call this function")));
      }

      callback_id = args[0]->Int32Value();

      Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());

      ret = virConnectDomainEventDeregisterAny(hypervisor->conn_, callback_id);

      if(ret == -1) {
          ThrowException(Error::New(virGetLastError()));
          return False();
      }

      return True();
  }

  int Hypervisor::domain_event_lifecycle_callback(virConnectPtr conn, //unused
                                                  virDomainPtr dom,
                                                  int event,
                                                  int detail,
                                                  void *opaque) {
      HandleScope scope;
      //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
      Domain *domain = new Domain();
      domain->domain_ = dom;
      Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
      domain->Wrap(domain_obj);

      // The virDomainPtr object handle passed into the callback upon delivery
      // of an event is only valid for the duration of execution of the callback
      // If the callback wishes to keep the domain object after the callback,
      // it shall take a reference to it, by calling virDomainRef

      virDomainRef(dom);

      Local<Value> argv[3];

      Persistent<Object> obj = static_cast<Object*>(opaque);
      Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

      Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
      Local<Function> callback = Local<Function>::Cast(callback_);

      Local<Object> data = Object::New();
      data->Set(domain_event_type_symbol, Integer::New(event));
      data->Set(domain_event_detail_symbol, Integer::New(detail));

      argv[0] = hyp;
      argv[1] = domain_obj; //FIXME change with domain->handle_
      argv[2] = data;

      TryCatch try_catch;

      callback->Call(hyp, 3, argv);

      if(try_catch.HasCaught()) {
          FatalException(try_catch);
      }

      return 0;
  }

  int Hypervisor::domain_event_generic_callback(  virConnectPtr conn, //unused
                                                  virDomainPtr dom,
                                                  void *opaque) {
      HandleScope scope;
      //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
      Domain *domain = new Domain();
      domain->domain_ = dom;
      Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
      domain->Wrap(domain_obj);

      // The virDomainPtr object handle passed into the callback upon delivery
      // of an event is only valid for the duration of execution of the callback
      // If the callback wishes to keep the domain object after the callback,
      // it shall take a reference to it, by calling virDomainRef

      virDomainRef(dom);

      Local<Value> argv[2];

      Persistent<Object> obj = static_cast<Object*>(opaque);
      Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

      Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
      Local<Function> callback = Local<Function>::Cast(callback_);

      argv[0] = hyp;
      argv[1] = domain_obj; //FIXME change with domain->handle_

      TryCatch try_catch;

      callback->Call(hyp, 2, argv);

      if(try_catch.HasCaught()) {
          FatalException(try_catch);
      }

      return 0;
  }

  int Hypervisor::domain_event_rtcchange_callback(virConnectPtr conn, //unused
                                                  virDomainPtr dom,
                                                  long long utcoffset,
                                                  void *opaque) {
      HandleScope scope;
      //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
      Domain *domain = new Domain();
      domain->domain_ = dom;
      Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
      domain->Wrap(domain_obj);

      // The virDomainPtr object handle passed into the callback upon delivery
      // of an event is only valid for the duration of execution of the callback
      // If the callback wishes to keep the domain object after the callback,
      // it shall take a reference to it, by calling virDomainRef

      virDomainRef(dom);

      Local<Value> argv[3];

      Persistent<Object> obj = static_cast<Object*>(opaque);
      Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

      Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
      Local<Function> callback = Local<Function>::Cast(callback_);

      Local<Object> data = Object::New();
      data->Set(domain_event_rtc_utcoffset_symbol, Number::New(utcoffset));

      argv[0] = hyp;
      argv[1] = domain_obj; //FIXME change with domain->handle_
      argv[2] = data;

      TryCatch try_catch;

      callback->Call(hyp, 3, argv);

      if(try_catch.HasCaught()) {
          FatalException(try_catch);
      }

      return 0;
  }

  int Hypervisor::domain_event_watchdog_callback( virConnectPtr conn, //unused
                                                  virDomainPtr dom,
                                                  int action,
                                                  void *opaque) {
      HandleScope scope;
      //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
      Domain *domain = new Domain();
      domain->domain_ = dom;
      Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
      domain->Wrap(domain_obj);

      // The virDomainPtr object handle passed into the callback upon delivery
      // of an event is only valid for the duration of execution of the callback
      // If the callback wishes to keep the domain object after the callback,
      // it shall take a reference to it, by calling virDomainRef

      virDomainRef(dom);

      Local<Value> argv[3];

      Persistent<Object> obj = static_cast<Object*>(opaque);
      Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

      Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
      Local<Function> callback = Local<Function>::Cast(callback_);

      Local<Object> data = Object::New();
      data->Set(domain_event_action_symbol, Integer::New(action));

      argv[0] = hyp;
      argv[1] = domain_obj; //FIXME change with domain->handle_
      argv[2] = data;

      TryCatch try_catch;

      callback->Call(hyp, 3, argv);

      if(try_catch.HasCaught()) {
          FatalException(try_catch);
      }

      return 0;
  }

  int Hypervisor::domain_event_io_error_callback( virConnectPtr conn, //unused
                                                  virDomainPtr dom,
                                                  const char *src_path,
                                                  const char *dev_alias,
                                                  int action,
                                                  void *opaque) {
      HandleScope scope;
      //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
      Domain *domain = new Domain();
      domain->domain_ = dom;
      Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
      domain->Wrap(domain_obj);

      // The virDomainPtr object handle passed into the callback upon delivery
      // of an event is only valid for the duration of execution of the callback
      // If the callback wishes to keep the domain object after the callback,
      // it shall take a reference to it, by calling virDomainRef

      virDomainRef(dom);

      Local<Value> argv[3];

      Persistent<Object> obj = static_cast<Object*>(opaque);
      Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

      Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
      Local<Function> callback = Local<Function>::Cast(callback_);

      Local<Object> data = Object::New();
      data->Set(domain_event_ioerror_srcpath_symbol, String::New(src_path));
      data->Set(domain_event_ioerror_devalias_symbol, String::New(dev_alias));
      data->Set(domain_event_action_symbol, Integer::New(action));

      argv[0] = hyp;
      argv[1] = domain_obj; //FIXME change with domain->handle_
      argv[2] = data;

      TryCatch try_catch;

      callback->Call(hyp, 3, argv);

      if(try_catch.HasCaught()) {
          FatalException(try_catch);
      }

      return 0;
  }

  int Hypervisor::domain_event_io_error_reason_callback(  virConnectPtr conn, //unused
                                                          virDomainPtr dom,
                                                          const char *src_path,
                                                          const char *dev_alias,
                                                          int action,
                                                          const char *reason,
                                                          void *opaque) {
      HandleScope scope;
      //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
      Domain *domain = new Domain();
      domain->domain_ = dom;
      Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
      domain->Wrap(domain_obj);

      // The virDomainPtr object handle passed into the callback upon delivery
      // of an event is only valid for the duration of execution of the callback
      // If the callback wishes to keep the domain object after the callback,
      // it shall take a reference to it, by calling virDomainRef
      virDomainRef(dom);

      Local<Value> argv[3];

      Persistent<Object> obj = static_cast<Object*>(opaque);
      Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

      Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
      Local<Function> callback = Local<Function>::Cast(callback_);

      Local<Object> data = Object::New();
      data->Set(domain_event_ioerror_srcpath_symbol, String::New(src_path));
      data->Set(domain_event_ioerror_devalias_symbol, String::New(dev_alias));
      data->Set(domain_event_ioerror_reason_symbol, String::New(reason));
      data->Set(domain_event_action_symbol, Integer::New(action));

      argv[0] = hyp;
      argv[1] = domain_obj; //FIXME change with domain->handle_
      argv[2] = data;

      TryCatch try_catch;

      callback->Call(hyp, 3, argv);

      if(try_catch.HasCaught()) {
          FatalException(try_catch);
      }

      return 0;
  }

  int Hypervisor::domain_event_graphics_callback( virConnectPtr conn, //unused
                                                  virDomainPtr dom,
                                                  int phase,
                                                  virDomainEventGraphicsAddressPtr local,
                                                  virDomainEventGraphicsAddressPtr remote,
                                                  const char *auth_scheme,
                                                  virDomainEventGraphicsSubjectPtr subject,
                                                  void *opaque) {
      HandleScope scope;
      //FIXME - Ugly code. DRY, maybe with Domain::New(dom) and domain->handle_
      Domain *domain = new Domain();
      domain->domain_ = dom;
      Local<Object> domain_obj = domain->constructor_template->GetFunction()->NewInstance();
      domain->Wrap(domain_obj);

      // The virDomainPtr object handle passed into the callback upon delivery
      // of an event is only valid for the duration of execution of the callback
      // If the callback wishes to keep the domain object after the callback,
      // it shall take a reference to it, by calling virDomainRef
      virDomainRef(dom);

      Local<Value> argv[3];

      Persistent<Object> obj = static_cast<Object*>(opaque);
      Local<Object> hyp = obj->Get(domain_event_hypervisor_symbol)->ToObject();

      Local<Value> callback_ = obj->Get(domain_event_callback_symbol);
      Local<Function> callback = Local<Function>::Cast(callback_);


      // {
      //   local: {family: ipv4, node: 127.0.0.1, service: 80},
      //   remote: {family: ipv6, node: ::1, service: 80},
      //   subject: [{type: password, name: foo}, {type: password, name: fooo}],
      //   phase: 0 //VIR_DOMAIN_EVENT_GRAPHICS_CONNECT
      //   auth_scheme: foooscheme
      // }

      Local<String> lfamily;
      switch(local->family) {
          case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV4:
              lfamily = String::New("ipv4");
              break;
          case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV6:
              lfamily = String::New("ipv6");
              break;

      };

      Local<String> rfamily;
      switch(remote->family) {
          case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV4:
              rfamily = String::New("ipv4");
              break;
          case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV6:
              rfamily = String::New("ipv6");
              break;

      };

      Local<Object> local_ = Object::New();
      local_->Set(domain_event_graphics_family_sym, lfamily);
      local_->Set(domain_event_graphics_node_sym, String::New(local->node));
      local_->Set(domain_event_graphics_service_sym, String::New(local->service));

      Local<Object> remote_ = Object::New();
      remote_->Set(domain_event_graphics_family_sym, rfamily);
      remote_->Set(domain_event_graphics_node_sym, String::New(remote->node));
      remote_->Set(domain_event_graphics_service_sym, String::New(remote->service));

      int nidentity = subject->nidentity;
      Local<Array> subject_ = Array::New(nidentity);

      for(int i = 0; i < nidentity; i++) {
          Local<Object> identity = Object::New();
          identity->Set(domain_event_graphics_subjtype_sym, String::New(subject->identities[i].type));
          identity->Set(domain_event_graphics_subjname_sym, String::New(subject->identities[i].name));

          subject_->Set(Integer::New(i), identity);
      }

      Local<Object> data = Object::New();
      data->Set(domain_event_graphics_local_sym, local_);
      data->Set(domain_event_graphics_remote_sym, remote_);
      data->Set(domain_event_graphics_subject_sym, subject_);
      data->Set(domain_event_graphics_phase_sym, Integer::New(phase));
      data->Set(domain_event_graphics_authscheme_sym, String::New(auth_scheme));

      argv[0] = hyp;
      argv[1] = domain_obj; //FIXME change with domain->handle_
      argv[2] = data;

      TryCatch try_catch;

      callback->Call(hyp, 3, argv);

      if(try_catch.HasCaught()) {
          FatalException(try_catch);
      }
      return 0;
  }

  void Hypervisor::domain_event_free(void *opaque) {

  }
*/