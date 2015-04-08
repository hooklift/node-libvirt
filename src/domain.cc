// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.

#include "error.h"
#include "hypervisor.h"
#include "domain.h"

namespace NodeLibvirt {

Persistent<FunctionTemplate> Domain::constructor_template;
void Domain::Initialize()
{
  Local<FunctionTemplate> t = FunctionTemplate::New();
  t->InstanceTemplate()->SetInternalFieldCount(1);

  // ACTIONS
  NODE_SET_PROTOTYPE_METHOD(t, "destroy",                 Destroy);
  NODE_SET_PROTOTYPE_METHOD(t, "start",                   Start);
  NODE_SET_PROTOTYPE_METHOD(t, "reboot",                  Reboot);
  NODE_SET_PROTOTYPE_METHOD(t, "shutdown",                Shutdown);
  NODE_SET_PROTOTYPE_METHOD(t, "reset",                   Reset);
  NODE_SET_PROTOTYPE_METHOD(t, "resume",                  Resume);
  NODE_SET_PROTOTYPE_METHOD(t, "suspend",                 Suspend);
  NODE_SET_PROTOTYPE_METHOD(t, "save",                    Save);
  NODE_SET_PROTOTYPE_METHOD(t, "managedSave",             ManagedSave);
  NODE_SET_PROTOTYPE_METHOD(t, "managedSaveRemove",       ManagedSaveRemove);
  NODE_SET_PROTOTYPE_METHOD(t, "abortCurrentJob",         AbortCurrentJob);
  NODE_SET_PROTOTYPE_METHOD(t, "coreDump",                CoreDump);

  NODE_SET_PROTOTYPE_METHOD(t, "attachDevice",            AttachDevice);
  NODE_SET_PROTOTYPE_METHOD(t, "detachDevice",            DetachDevice);
  NODE_SET_PROTOTYPE_METHOD(t, "updateDevice",            UpdateDevice);
  NODE_SET_PROTOTYPE_METHOD(t, "sendKeys",                SendKeys);

  // UNFINISHED SYNC ACTIONS
  NODE_SET_PROTOTYPE_METHOD(t, "memoryPeek",              MemoryPeek);
  NODE_SET_PROTOTYPE_METHOD(t, "blockPeek",               BlockPeek);
  NODE_SET_PROTOTYPE_METHOD(t, "migrate",                 Migrate);
  NODE_SET_PROTOTYPE_METHOD(t, "pinVcpu",                 PinVcpu);

  // ACCESSORS/MUTATORS
  NODE_SET_PROTOTYPE_METHOD(t, "getName",                 GetName);
  NODE_SET_PROTOTYPE_METHOD(t, "getInfo",                 GetInfo);
  NODE_SET_PROTOTYPE_METHOD(t, "getId",                   GetId);
  NODE_SET_PROTOTYPE_METHOD(t, "getOSType",               GetOSType);
  NODE_SET_PROTOTYPE_METHOD(t, "getUUID",                 GetUUID);
  NODE_SET_PROTOTYPE_METHOD(t, "getAutostart",            GetAutostart);
  NODE_SET_PROTOTYPE_METHOD(t, "setAutostart",            SetAutostart);
  NODE_SET_PROTOTYPE_METHOD(t, "getMaxMemory",            GetMaxMemory);
  NODE_SET_PROTOTYPE_METHOD(t, "setMaxMemory",            SetMaxMemory);
  NODE_SET_PROTOTYPE_METHOD(t, "setMemory",               SetMemory);
  NODE_SET_PROTOTYPE_METHOD(t, "getMaxVcpus",             GetMaxVcpus);
  NODE_SET_PROTOTYPE_METHOD(t, "isActive",                IsActive);
  NODE_SET_PROTOTYPE_METHOD(t, "isPersistent",            IsPersistent);
  NODE_SET_PROTOTYPE_METHOD(t, "isUpdated",               IsUpdated);
  NODE_SET_PROTOTYPE_METHOD(t, "toXml",                   ToXml);
  NODE_SET_PROTOTYPE_METHOD(t, "getBlockInfo",            GetBlockInfo);
  NODE_SET_PROTOTYPE_METHOD(t, "getBlockStats",           GetBlockStats);
  NODE_SET_PROTOTYPE_METHOD(t, "getSchedulerType",        GetSchedulerType);
  NODE_SET_PROTOTYPE_METHOD(t, "getSchedulerParameters",  GetSchedulerParameters);
  NODE_SET_PROTOTYPE_METHOD(t, "getSecurityLabel",        GetSecurityLabel);
  NODE_SET_PROTOTYPE_METHOD(t, "hasManagedSaveImage",     HasManagedSaveImage);
  NODE_SET_PROTOTYPE_METHOD(t, "getInterfaceStats",       GetInterfaceStats);
  NODE_SET_PROTOTYPE_METHOD(t, "getJobInfo",              GetJobInfo);
  NODE_SET_PROTOTYPE_METHOD(t, "getMemoryStats",          GetMemoryStats);
  NODE_SET_PROTOTYPE_METHOD(t, "getVcpus",                GetVcpus);
  NODE_SET_PROTOTYPE_METHOD(t, "setVcpus",                SetVcpus);

  // UNFINISHED SYNC ACCESSORS/MUTATORS
  NODE_SET_PROTOTYPE_METHOD(t, "setSchedulerParameters",  SetSchedulerParameters);
  NODE_SET_PROTOTYPE_METHOD(t, "setMigrationMaxDowntime", SetMigrationMaxDowntime);
  NODE_SET_PROTOTYPE_METHOD(t, "hasCurrentSnapshot",      HasCurrentSnapshot);
  NODE_SET_PROTOTYPE_METHOD(t, "revertToSnapshot",        RevertToSnapshot);
  NODE_SET_PROTOTYPE_METHOD(t, "takeSnapshot",            TakeSnapshot);
  NODE_SET_PROTOTYPE_METHOD(t, "getCurrentSnapshot",      GetCurrentSnapshot);
  NODE_SET_PROTOTYPE_METHOD(t, "deleteSnapshot",          DeleteSnapshot);
  NODE_SET_PROTOTYPE_METHOD(t, "lookupSnapshotByName",    LookupSnapshotByName);
  NODE_SET_PROTOTYPE_METHOD(t, "getSnapshots",            GetSnapshots);

  NanAssignPersistent(constructor_template, t);
  constructor_template->SetClassName(NanNew("Domain"));

  Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

  //Constants initialization
  //virDomainState
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_NOSTATE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_RUNNING);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_BLOCKED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_PAUSED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SHUTDOWN);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SHUTOFF);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_CRASHED);

#ifdef VIR_DOMAIN_PMSUSPENDED
  // If its available in libvirt.h, then make it available in node
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_PMSUSPENDED);
#endif

  //virDomainDeviceModifyFlags
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_DEVICE_MODIFY_CURRENT);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_DEVICE_MODIFY_LIVE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_DEVICE_MODIFY_CONFIG);

  //virDomainMigrateFlags
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_LIVE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_PEER2PEER);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_TUNNELLED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_PERSIST_DEST);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_UNDEFINE_SOURCE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_PAUSED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_NON_SHARED_DISK);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MIGRATE_NON_SHARED_INC);

  //virDomainXMLFlags
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_XML_SECURE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_XML_INACTIVE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_XML_UPDATE_CPU);

  //virDomainJobType
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_NONE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_BOUNDED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_UNBOUNDED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_COMPLETED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_FAILED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_JOB_CANCELLED);

  //virDomainMemoryFlags
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MEMORY_VIRTUAL);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_MEMORY_PHYSICAL);

  //virDomainEventType
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_DEFINED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_UNDEFINED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_RESUMED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED);

  //virDomainEventIOErrorAction
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_IO_ERROR_NONE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_IO_ERROR_PAUSE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_IO_ERROR_REPORT);

  //virDomainEventResumedDetailType
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_RESUMED_UNPAUSED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_RESUMED_MIGRATED);

  //virDomainEventStartedDetailType
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_BOOTED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_MIGRATED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_RESTORED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STARTED_FROM_SNAPSHOT);

  //virDomainEventStoppedDetailType
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_SHUTDOWN);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_DESTROYED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_CRASHED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_MIGRATED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_SAVED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_FAILED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_STOPPED_FROM_SNAPSHOT);

  //virDomainEventSuspendedDetailType
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_PAUSED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_MIGRATED);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_IOERROR);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_SUSPENDED_WATCHDOG);

  //virDomainEventUndefinedDetailType
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_UNDEFINED_REMOVED);

  //virDomainEventWatchdogAction
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_NONE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_PAUSE);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_RESET);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_POWEROFF);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_SHUTDOWN);
  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_EVENT_WATCHDOG_DEBUG);

  NODE_DEFINE_CONSTANT(object_tmpl, VIR_DOMAIN_SEND_KEY_MAX_KEYS);
}

Local<Object> Domain::NewInstance(const LibVirtHandle &handle)
{
  NanScope();
  Domain *domain = new Domain(handle.ToDomain());
  Local<Object> object = constructor_template->GetFunction()->NewInstance();
  domain->Wrap(object);
  return NanEscapeScope(object);
}

Domain::~Domain()
{
  fprintf(stderr, "\n\nDELETING THE DOMAIN!!!\n\n");

  if (handle_ != NULL)
    virDomainFree(handle_);
  handle_ = 0;
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Domain, LookupByName, virDomainLookupByName)
NAN_METHOD(Domain::LookupByName)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid domain name and callback.");
    NanReturnUndefined();
  }

  if (!NanHasInstance(Hypervisor::constructor_template, args.This())) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  std::string name(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByNameWorker(callback, hv->handle_, name));
  NanReturnUndefined();
}

NLV_LOOKUP_BY_VALUE_EXECUTE_IMPL(Domain, LookupByUUID, virDomainLookupByUUIDString)
NAN_METHOD(Domain::LookupByUUID)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid domain uuid and callback.");
    NanReturnUndefined();
  }

  if (!NanHasInstance(Hypervisor::constructor_template, args.This())) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  std::string uuid(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByUUIDWorker(callback, hv->handle_, uuid));
  NanReturnUndefined();
}

NAN_METHOD(Domain::LookupById)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsInt32() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a valid domain id and callback.");
    NanReturnUndefined();
  }

  if (!NanHasInstance(Hypervisor::constructor_template, args.This())) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(args.This());
  int id = args[0]->IntegerValue();
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new LookupByIdWorker(callback, hv->handle_, id));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, LookupById)
{
  NLV_WORKER_ASSERT_CONNECTION();
  lookupHandle_ = virDomainLookupByID(Handle().ToConnection(), id_);
  if (lookupHandle_.ToDomain() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_CREATE(Domain)
NLV_WORKER_EXECUTE(Domain, Create)
{
  NLV_WORKER_ASSERT_CONNECTION();

  unsigned int flags = 0;
  lookupHandle_ =
    virDomainCreateXML(Handle().ToConnection(), value_.c_str(), flags);
  if (lookupHandle_.ToDomain() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_DEFINE(Domain)
NLV_WORKER_EXECUTE(Domain, Define)
{
  NLV_WORKER_ASSERT_CONNECTION();

  lookupHandle_ =
    virDomainDefineXML(Handle().ToConnection(), value_.c_str());
  if (lookupHandle_.ToDomain() == NULL) {
    SetVirError(virGetLastError());
    return;
  }
}

NAN_METHOD(Domain::Save)
{
  NanScope();
  if (args.Length() != 2 &&
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a path and callback");
    NanReturnUndefined();
  }

  std::string path(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new SaveWorker(callback, domain->handle_, path));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, Save)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainSave(Handle().ToDomain(), path_.c_str());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(Domain::Restore)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a path and callback");
    NanReturnUndefined();
  }

  Local<Object> object = args.This();
  if (!NanHasInstance(Hypervisor::constructor_template, object)) {
    NanThrowTypeError("You must specify a Hypervisor instance");
    NanReturnUndefined();
  }

  std::string path(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Hypervisor *hv = ObjectWrap::Unwrap<Hypervisor>(object);
  NanAsyncQueueWorker(new RestoreWorker(callback, hv->handle_, path));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, Restore)
{
  NLV_WORKER_ASSERT_CONNECTION();
  int result = virDomainRestore(Handle().ToConnection(), path_.c_str());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(Domain::CoreDump)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a path and callback");
    NanReturnUndefined();
  }

  std::string path(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new CoreDumpWorker(callback, domain->handle_, path));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, CoreDump)
{
  NLV_WORKER_ASSERT_DOMAIN();

  unsigned int flags = 0;
  int result = virDomainCoreDump(Handle().ToDomain(), path_.c_str(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, Destroy)
NLV_WORKER_EXECUTE(Domain, Destroy)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainDestroy(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  if (Handle().ToDomain() != NULL) {
    Handle().Clear();
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, ManagedSave)
NLV_WORKER_EXECUTE(Domain, ManagedSave)
{
  NLV_WORKER_ASSERT_DOMAIN();

  unsigned int flags = 0;
  int result = virDomainManagedSave(Handle().ToDomain(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, ManagedSaveRemove)
NLV_WORKER_EXECUTE(Domain, ManagedSaveRemove)
{
  NLV_WORKER_ASSERT_DOMAIN();

  unsigned int flags = 0;
  int result = virDomainManagedSaveRemove(Handle().ToDomain(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetName)
NLV_WORKER_EXECUTE(Domain, GetName)
{
  NLV_WORKER_ASSERT_DOMAIN();
  const char *result = virDomainGetName(Handle().ToDomain());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetId)
NLV_WORKER_EXECUTE(Domain, GetId)
{
  NLV_WORKER_ASSERT_DOMAIN();

  int result = virDomainGetID(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetOSType)
NLV_WORKER_EXECUTE(Domain, GetOSType)
{
  NLV_WORKER_ASSERT_DOMAIN();

  const char *result = virDomainGetOSType(Handle().ToDomain());
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetUUID)
NLV_WORKER_EXECUTE(Domain, GetUUID)
{
  NLV_WORKER_ASSERT_DOMAIN();

  char *uuid = new char[VIR_UUID_STRING_BUFLEN];
  int result = virDomainGetUUIDString(Handle().ToDomain(), uuid);
  if (result == -1) {
    SetVirError(virGetLastError());
    delete[] uuid;
    return;
  }

  data_ = uuid;
  delete[] uuid;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetAutostart)
NLV_WORKER_EXECUTE(Domain, GetAutostart)
{
  NLV_WORKER_ASSERT_DOMAIN();

  int autostart;
  int result = virDomainGetAutostart(Handle().ToDomain(), &autostart);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(autostart);
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetMaxMemory)
NLV_WORKER_EXECUTE(Domain, GetMaxMemory)
{
  NLV_WORKER_ASSERT_DOMAIN();
  unsigned long result = virDomainGetMaxMemory(Handle().ToDomain());
  if (result == 0) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetMaxVcpus)
NLV_WORKER_EXECUTE(Domain, GetMaxVcpus)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainGetMaxVcpus(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, IsActive)
NLV_WORKER_EXECUTE(Domain, IsActive)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainIsActive(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_NO_ARGS(Domain, IsPersistent)
NLV_WORKER_EXECUTE(Domain, IsPersistent)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainIsPersistent(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NLV_WORKER_METHOD_NO_ARGS(Domain, IsUpdated)
NLV_WORKER_EXECUTE(Domain, IsUpdated)
{
  NLV_WORKER_ASSERT_DOMAIN();

  int result = virDomainIsUpdated(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}


NLV_WORKER_METHOD_NO_ARGS(Domain, Start)
NLV_WORKER_EXECUTE(Domain, Start)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainCreate(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, Reboot)
NLV_WORKER_EXECUTE(Domain, Reboot)
{
  NLV_WORKER_ASSERT_DOMAIN();

  unsigned long flags = 0;
  int result = virDomainReboot(Handle().ToDomain(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, Reset)
NLV_WORKER_EXECUTE(Domain, Reset)
{
  NLV_WORKER_ASSERT_DOMAIN();

  unsigned long flags = 0;
  int result = virDomainReset(Handle().ToDomain(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, Shutdown)
NLV_WORKER_EXECUTE(Domain, Shutdown)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainShutdown(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, Suspend)
NLV_WORKER_EXECUTE(Domain, Suspend)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainSuspend(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, Resume)
NLV_WORKER_EXECUTE(Domain, Resume)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainResume(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, HasManagedSaveImage)
NLV_WORKER_EXECUTE(Domain, HasManagedSaveImage)
{
  NLV_WORKER_ASSERT_DOMAIN();

  unsigned long flags = 0;
  int result = virDomainHasManagedSaveImage(Handle().ToDomain(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = static_cast<bool>(result);
}

NAN_METHOD(Domain::SetAutostart)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a bool and callback");
    NanReturnUndefined();
  }

  bool autoStart = args[0]->IsTrue();
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new SetAutostartWorker(callback, domain->handle_, autoStart));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, SetAutostart)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainSetAutostart(Handle().ToDomain(), autoStart_ ? 1 : 0);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(Domain::SetMaxMemory)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsNumber() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a bool and callback");
    NanReturnUndefined();
  }

  unsigned long maxMemory = static_cast<unsigned long>(args[0]->NumberValue());
  // @todo this unsafe, also needs bounds checking!
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new SetMaxMemoryWorker(callback, domain->handle_, maxMemory));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, SetMaxMemory)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainSetMaxMemory(Handle().ToDomain(), maxMemory_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(Domain::SetMemory)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsNumber() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a bool and callback");
    NanReturnUndefined();
  }

  unsigned long memory = static_cast<unsigned long>(args[0]->NumberValue());
  // @todo this unsafe, also needs bounds checking!
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new SetMemoryWorker(callback, domain->handle_, memory));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, SetMemory)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainSetMemory(Handle().ToDomain(), memory_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}


NLV_WORKER_METHOD_NO_ARGS(Domain, ToXml)
NLV_WORKER_EXECUTE(Domain, ToXml)
{
  NLV_WORKER_ASSERT_DOMAIN();

  unsigned int flags = 0;
  char *result = virDomainGetXMLDesc(Handle().ToDomain(), flags);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetInfo)
NLV_WORKER_EXECUTE(Domain, GetInfo)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainGetInfo(Handle().ToDomain(), &info_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Domain, GetInfo)
{
  NanScope();
  Local<Object> result = NanNew<Object>();
  result->Set(NanNew("state"), NanNew<Integer>(info_.state));
  result->Set(NanNew("maxMemory"), NanNew<Number>(info_.maxMem));
  result->Set(NanNew("memory"), NanNew<Number>(info_.memory));
  result->Set(NanNew("vcpus"), NanNew<Integer>(info_.nrVirtCpu));
  result->Set(NanNew("cpuTime"), NanNew<Number>(info_.cpuTime));

  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NAN_METHOD(Domain::GetBlockInfo)
{
  NanScope();
  if (args.Length() != 2 &&
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a path and callback");
    NanReturnUndefined();
  }

  std::string path(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new GetBlockInfoWorker(callback, domain->handle_, path));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, GetBlockInfo)
{
  NLV_WORKER_ASSERT_DOMAIN();
  unsigned int flags = 0;
  int result = virDomainGetBlockInfo(Handle().ToDomain(), path_.c_str(), &info_, flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Domain, GetBlockInfo)
{
  NanScope();
  Local<Object> result = NanNew<Object>();
  result->Set(NanNew("capacity"), NanNew<Number>(info_.capacity));
  result->Set(NanNew("allocation"), NanNew<Number>(info_.allocation));
  result->Set(NanNew("physical"), NanNew<Number>(info_.physical));

  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NAN_METHOD(Domain::GetBlockStats)
{
  NanScope();
  if (args.Length() != 2 &&
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a path and callback");
    NanReturnUndefined();
  }

  std::string path(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new GetBlockStatsWorker(callback, domain->handle_, path));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, GetBlockStats)
{
  NLV_WORKER_ASSERT_DOMAIN();

  int result =
    virDomainBlockStats(Handle().ToDomain(), path_.c_str(), &stats_, sizeof(stats_));
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Domain, GetBlockStats)
{
  NanScope();
  Local<Object> result = NanNew<Object>();
  result->Set(NanNew("readRequests"), NanNew<Number>(stats_.rd_req));
  result->Set(NanNew("readBytes"), NanNew<Number>(stats_.rd_bytes));
  result->Set(NanNew("writeRequests"), NanNew<Number>(stats_.wr_req));
  result->Set(NanNew("writeBytes"), NanNew<Number>(stats_.wr_bytes));
  result->Set(NanNew("errors"), NanNew<Number>(stats_.errs));

  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetSchedulerType)
NLV_WORKER_EXECUTE(Domain, GetSchedulerType)
{
  NLV_WORKER_ASSERT_DOMAIN();
  char *result = virDomainGetSchedulerType(Handle().ToDomain(), NULL);
  if (result == NULL) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = result;
  free(result);
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetSchedulerParameters)
NLV_WORKER_EXECUTE(Domain, GetSchedulerParameters)
{
  NLV_WORKER_ASSERT_DOMAIN();

  // get number of parameters
  int numParams;
  char *type = virDomainGetSchedulerType(Handle().ToDomain(), &numParams);
  if (type == NULL) {
    SetVirError(virGetLastError());
    return;
  }
  free(type);

  params_.resize(numParams);
  int result = virDomainGetSchedulerParameters(Handle().ToDomain(), params_.data(), &numParams);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetSecurityLabel)
NLV_WORKER_EXECUTE(Domain, GetSecurityLabel)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainGetSecurityLabel(Handle().ToDomain(), &info_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Domain, GetSecurityLabel)
{
  NanScope();
  Local<Object> result = NanNew<Object>();
  result->Set(NanNew("label"), NanNew<String>(info_.label));
  result->Set(NanNew("enforcing"), NanNew<Boolean>(info_.enforcing));

  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NAN_METHOD(Domain::GetInterfaceStats)
{
  NanScope();
  if (args.Length() != 2 &&
      (!args[0]->IsString() && !args[1]->IsFunction())) {
    NanThrowTypeError("You must specify a device and callback");
    NanReturnUndefined();
  }

  std::string interface(*NanUtf8String(args[0]->ToString()));
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new GetInterfaceStatsWorker(callback, domain->handle_, interface));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, GetInterfaceStats)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result =
    virDomainInterfaceStats(Handle().ToDomain(), interface_.c_str(), &stats_, sizeof(&stats_));
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Domain, GetInterfaceStats)
{
  NanScope();
  Local<Object> rx = NanNew<Object>();
  rx->Set(NanNew("bytes"), NanNew<Number>(stats_.rx_bytes));
  rx->Set(NanNew("packets"), NanNew<Number>(stats_.rx_packets));
  rx->Set(NanNew("errors"), NanNew<Number>(stats_.rx_errs));
  rx->Set(NanNew("drop"), NanNew<Number>(stats_.rx_drop));

  Local<Object> tx = NanNew<Object>();
  tx->Set(NanNew("bytes"), NanNew<Number>(stats_.tx_bytes));
  tx->Set(NanNew("packets"), NanNew<Number>(stats_.tx_packets));
  tx->Set(NanNew("errors"), NanNew<Number>(stats_.tx_errs));
  tx->Set(NanNew("drop"), NanNew<Number>(stats_.tx_drop));

  Local<Object> result = NanNew<Object>();
  result->Set(NanNew("rx"), rx);
  result->Set(NanNew("tx"), tx);

  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetJobInfo)
NLV_WORKER_EXECUTE(Domain, GetJobInfo)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainGetJobInfo(Handle().ToDomain(), &info_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Domain, GetJobInfo)
{
  NanScope();
  Local<Object> time = NanNew<Object>();
  time->Set(NanNew("elapsed"), NanNew<Number>(info_.timeElapsed));
  time->Set(NanNew("remaining"), NanNew<Number>(info_.timeRemaining));

  Local<Object> data = NanNew<Object>();
  data->Set(NanNew("total"), NanNew<Number>(info_.dataTotal));
  data->Set(NanNew("processed"), NanNew<Number>(info_.dataProcessed));
  data->Set(NanNew("remaining"), NanNew<Number>(info_.dataRemaining));

  Local<Object> memory = NanNew<Object>();
  memory->Set(NanNew("total"), NanNew<Number>(info_.memTotal));
  memory->Set(NanNew("processed"), NanNew<Number>(info_.memProcessed));
  memory->Set(NanNew("remaining"), NanNew<Number>(info_.memRemaining));

  Local<Object> file = NanNew<Object>();
  file->Set(NanNew("total"), NanNew<Number>(info_.fileTotal));
  file->Set(NanNew("processed"), NanNew<Number>(info_.fileProcessed));
  file->Set(NanNew("remaining"), NanNew<Number>(info_.fileRemaining));

  Local<Object> result = NanNew<Object>();
  result->Set(NanNew("type"), NanNew<Integer>(info_.type));
  result->Set(NanNew("time"), time);
  result->Set(NanNew("data"), data);
  result->Set(NanNew("memory"), memory);
  result->Set(NanNew("file"), file);

  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NLV_WORKER_METHOD_NO_ARGS(Domain, AbortCurrentJob)
NLV_WORKER_EXECUTE(Domain, AbortCurrentJob)
{
  NLV_WORKER_ASSERT_DOMAIN();
  int result = virDomainAbortJob(Handle().ToDomain());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetMemoryStats)
NLV_WORKER_EXECUTE(Domain, GetMemoryStats)
{
  NLV_WORKER_ASSERT_DOMAIN();

  unsigned int flags = 0;
  stats_.resize(VIR_DOMAIN_MEMORY_STAT_NR);
  int result = virDomainMemoryStats(Handle().ToDomain(), stats_.data(), stats_.size(), flags);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Domain, GetMemoryStats)
{
  NanScope();
  Local<Object> result = NanNew<Object>();
  for(unsigned int i = 0; i < stats_.size(); ++i) {
    switch (stats_[i].tag) {
    case VIR_DOMAIN_MEMORY_STAT_SWAP_IN:
      result->Set(NanNew("swapIn"), NanNew<Number>(stats_[i].val));
      break;
    case VIR_DOMAIN_MEMORY_STAT_SWAP_OUT:
      result->Set(NanNew("swapOut"), NanNew<Number>(stats_[i].val));
      break;
    case VIR_DOMAIN_MEMORY_STAT_MAJOR_FAULT:
      result->Set(NanNew("majorFault"), NanNew<Number>(stats_[i].val));
      break;
    case VIR_DOMAIN_MEMORY_STAT_MINOR_FAULT:
      result->Set(NanNew("minorFault"), NanNew<Number>(stats_[i].val));
      break;
    case VIR_DOMAIN_MEMORY_STAT_UNUSED:
      result->Set(NanNew("unused"), NanNew<Number>(stats_[i].val));
      break;
    case VIR_DOMAIN_MEMORY_STAT_AVAILABLE:
      result->Set(NanNew("available"), NanNew<Number>(stats_[i].val));
      break;
    }
  }

  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NAN_METHOD(Domain::AttachDevice)
{
  NanScope();
  if (args.Length() < 2 ||
      (args.Length() == 2 && (!args[0]->IsString() && !args[1]->IsFunction())) ||
      (args.Length() == 3 && (!args[0]->IsString() && !args[1]->IsArray() && !args[2]->IsFunction()))) {
    NanThrowTypeError("you must at least specify xml and a callback");
    NanReturnUndefined();
  }

  NanCallback *callback;
  std::string xml(*NanUtf8String(args[0]->ToString()));
  unsigned long flags = 0;

  if (args[1]->IsArray()) {
    Local<Array> flagsArray = args[1].As<Array>();
    for (unsigned int i = 0; i < flagsArray->Length(); ++i)
      flags |= flagsArray->Get(NanNew(i))->Int32Value();
    callback = new NanCallback(args[2].As<Function>());
  } else {
    callback = new NanCallback(args[1].As<Function>());
  }

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new AttachDeviceWorker(callback, domain->handle_, xml, flags));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, AttachDevice)
{
  NLV_WORKER_ASSERT_DOMAIN();

  int result;
  if (flags_ > 0) {
    result = virDomainAttachDeviceFlags(Handle().ToDomain(), xml_.c_str(), flags_);
  } else {
    result = virDomainAttachDevice(Handle().ToDomain(), xml_.c_str());
  }

  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(Domain::DetachDevice)
{
  NanScope();
  if (args.Length() < 2 ||
      (args.Length() == 2 && (!args[0]->IsString() && !args[1]->IsFunction())) ||
      (args.Length() == 3 && (!args[0]->IsString() && !args[1]->IsArray() && !args[2]->IsFunction()))) {
    NanThrowTypeError("you must at least specify xml and a callback");
    NanReturnUndefined();
  }

  NanCallback *callback;
  std::string xml(*NanUtf8String(args[0]->ToString()));
  unsigned long flags = 0;

  if (args[1]->IsArray()) {
    Local<Array> flagsArray = args[1].As<Array>();
    for (unsigned int i = 0; i < flagsArray->Length(); ++i)
      flags |= flagsArray->Get(NanNew(i))->Int32Value();
    callback = new NanCallback(args[2].As<Function>());
  } else {
    callback = new NanCallback(args[1].As<Function>());
  }

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new DetachDeviceWorker(callback, domain->handle_, xml, flags));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, DetachDevice)
{
  NLV_WORKER_ASSERT_DOMAIN();

  int result;
  if (flags_ > 0) {
    result = virDomainDetachDeviceFlags(Handle().ToDomain(), xml_.c_str(), flags_);
  } else {
    result = virDomainDetachDevice(Handle().ToDomain(), xml_.c_str());
  }

  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(Domain::UpdateDevice)
{
  NanScope();
  if (args.Length() < 2 ||
      (args.Length() == 2 && (!args[0]->IsString() && !args[1]->IsFunction())) ||
      (args.Length() == 3 && (!args[0]->IsString() && !args[1]->IsArray() && !args[2]->IsFunction()))) {
    NanThrowTypeError("you must at least specify xml and a callback");
    NanReturnUndefined();
  }

  NanCallback *callback;
  std::string xml(*NanUtf8String(args[0]->ToString()));
  unsigned long flags = 0;

  if (args[1]->IsArray()) {
    Local<Array> flagsArray = args[1].As<Array>();
    for (unsigned int i = 0; i < flagsArray->Length(); ++i)
      flags |= flagsArray->Get(NanNew(i))->Int32Value();
    callback = new NanCallback(args[2].As<Function>());
  } else {
    callback = new NanCallback(args[1].As<Function>());
  }

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanAsyncQueueWorker(new UpdateDeviceWorker(callback, domain->handle_, xml, flags));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, UpdateDevice)
{
  NLV_WORKER_ASSERT_DOMAIN();

  int result = virDomainUpdateDeviceFlags(Handle().ToDomain(), xml_.c_str(), flags_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NLV_WORKER_METHOD_NO_ARGS(Domain, GetVcpus)
NLV_WORKER_EXECUTE(Domain, GetVcpus)
{
  NLV_WORKER_ASSERT_DOMAIN();

  virDomainInfo info;
  int result = virDomainGetInfo(Handle().ToDomain(), &info);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  result = virNodeGetInfo(virDomainGetConnect(Handle().ToDomain()), &nodeInfo_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  int cpuMapLen = VIR_CPU_MAPLEN(VIR_NODEINFO_MAXCPUS(nodeInfo_));
  map_.resize(info.nrVirtCpu * cpuMapLen, 0);
  info_.resize(info.nrVirtCpu);
  result =
    virDomainGetVcpus(Handle().ToDomain(), info_.data(), info_.size(), map_.data(), map_.size());
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }
}

NLV_WORKER_OKCALLBACK(Domain, GetVcpus)
{
  NanScope();

  Local<Array> result = NanNew<Array>(info_.size());
  for (unsigned int i = 0; i < info_.size(); ++i) {
    Local<Object> cpu = NanNew<Object>();
    cpu->Set(NanNew("number"), NanNew<Integer>(info_[i].number));
    cpu->Set(NanNew("state"), NanNew<Integer>(info_[i].state));
    cpu->Set(NanNew("cpuTime"), NanNew<Number>(info_[i].cpuTime));
    cpu->Set(NanNew("cpu"), NanNew<Integer>(info_[i].cpu));

    int maxCpus = VIR_NODEINFO_MAXCPUS(nodeInfo_);
    Local<Array> affinity = NanNew<Array>(maxCpus);
    for (unsigned int j = 0; j < maxCpus; ++j) {
      Local<Object> realCpu = NanNew<Object>();
      realCpu->Set(NanNew("cpu"), NanNew(j));
      realCpu->Set(NanNew("usable"), NanNew<Boolean>(VIR_CPU_USABLE(map_, map_.size(), i, j)));
      affinity->Set(NanNew(j), realCpu);
    }

    cpu->Set(NanNew("affinity"), affinity);
    result->Set(NanNew(i), cpu);
  }

  v8::Local<v8::Value> argv[] = { NanNull(), result };
  callback->Call(2, argv);
}

NAN_METHOD(Domain::SetVcpus)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsInt32() && !args[1]->IsFunction())) {
    NanThrowTypeError("you must specify a count and a callback");
    NanReturnUndefined();
  }

  unsigned int count = args[0]->Int32Value();
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new SetVcpusWorker(callback, domain->handle_, count));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, SetVcpus)
{
  NLV_WORKER_ASSERT_DOMAIN();

  int result = virDomainSetVcpus(Handle().ToDomain(), count_);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(Domain::SendKeys)
{
  NanScope();
  if (args.Length() < 2 ||
      (!args[0]->IsArray() && !args[1]->IsFunction())) {
    NanThrowTypeError("you must specify an array of keycodes and a callback");
    NanReturnUndefined();
  }

  Local<Array> keyCodes = args[0].As<Array>();
  std::vector<unsigned int> keys;
  for (unsigned int i = 0; i < keyCodes->Length(); ++i) {
    keys.push_back((unsigned int) keyCodes->Get(NanNew(i))->Int32Value());
  }


  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanCallback *callback = new NanCallback(args[1].As<Function>());
  NanAsyncQueueWorker(new SendKeysWorker(callback, domain->handle_, keys));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, SendKeys)
{
  NLV_WORKER_ASSERT_DOMAIN();

  int result =
    virDomainSendKey(Handle().ToDomain(), 0, 150, keys_.data(), keys_.size(), 0);
  if (result == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

NAN_METHOD(Domain::Migrate)
{
  NanScope();
  unsigned long flags = 0;
  unsigned long bandwidth = 0;

  if(args.Length() < 2 ||
      (!args[0]->IsObject() && !args[1]->IsFunction())) {
    NanThrowTypeError("you must specify an object and a callback");
    NanReturnUndefined();
  }

  Local<Object> args_ = args[0]->ToObject();

  if(!args_->Has(NanNew("dest_uri"))) {
    NanThrowTypeError("You must have set property dest_uri in the object");
    NanReturnUndefined();
  }

  std::string dest_uri(*NanUtf8String(args_->Get(NanNew("dest_uri"))));
  std::string dest_name(*NanUtf8String(args_->Get(NanNew("dest_name"))));

  if(args_->Has(NanNew("flags"))) {
    Local<Array> flags_ = args_->Get(NanNew("flags")).As<Array>();
    unsigned int length = flags_->Length();
    for (unsigned int i = 0; i < length; i++)
      flags |= flags_->Get(Integer::New(i))->Int32Value();
  }

  if(args_->Has(NanNew("bandwidth"))) {
    bandwidth = args_->Get(NanNew("bandwidth"))->Int32Value();
  }

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

  NanCallback *callback = new NanCallback(args[1].As<Function>());
  MigrateWorker *worker;

  if(args_->Has(NanNew("dest_hypervisor"))) {
    Local<Object> hyp_obj = args_->Get(NanNew("dest_hypervisor"))->ToObject();
    if(!NanHasInstance(Hypervisor::constructor_template, hyp_obj)) {
      NanThrowTypeError("You must specify a Hypervisor object instance");
      NanReturnUndefined();
    }

    Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);
    worker = new MigrateWorker(callback, domain->handle_, hypervisor->handle_);
  } else {
    worker = new MigrateWorker(callback, domain->handle_, dest_uri);
  }

  worker->setBandwidth(bandwidth);
  worker->setFlags(flags);
  worker->setDestname(dest_name);
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, Migrate)
{
  NLV_WORKER_ASSERT_DOMAIN();

  if(conn_) {
    migrated_ = virDomainMigrate(Handle().ToDomain(), conn_, flags_, destname_.c_str(), uri_.c_str(), bandwidth_);
    if(migrated_ == NULL) {
      SetVirError(virGetLastError());
      return;
    }
  } else {
    int ret = -1;
    ret = virDomainMigrateToURI(Handle().ToDomain(), uri_.c_str(), flags_, destname_.c_str(), bandwidth_);
    if(ret == -1) {
      SetVirError(virGetLastError());
      return;
    }
  }
}

NLV_WORKER_OKCALLBACK(Domain, Migrate)
{
  NanScope();

  if (migrated_ != NULL) {
    Local<Object> domain_obj = Domain::NewInstance(migrated_);
    Local<Value> argv[] = { NanNull(), domain_obj };
    callback->Call(2, argv);
  } else {
    callback->Call(0, NULL);
  }
}

NAN_METHOD(Domain::PinVcpu) {
  NanScope();

  if(args.Length() < 3 ||
      (!args[0]->IsInt32() && !args[1]->IsArray() && !args[2]->IsFunction())) {
    NanThrowTypeError("you must specify an integer, an array and a callback");
    NanReturnUndefined();
  }

  std::vector<bool> usables;
  std::vector<int> vcpus;

  Local<Array> cpus = args[1].As<Array>();

  for(int i = 0; i < cpus->Length(); i++) {
    if(!cpus->Get(NanNew<Integer>(i))->IsObject()) {
      NanThrowTypeError("The second argument must be an array of objects");
      NanReturnUndefined();
    }

    Local<Object> cpu = cpus->Get(NanNew<Integer>(i))->ToObject();

    usables.push_back(cpu->Get(NanNew("usable"))->IsTrue());
    vcpus.push_back(cpu->Get(NanNew("cpu"))->Int32Value());
  }

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  NanCallback *callback = new NanCallback(args[2].As<Function>());
  NanAsyncQueueWorker(new PinVcpuWorker(callback, domain->handle_, args[0]->Int32Value(), usables, vcpus));
  NanReturnUndefined();
}

NLV_WORKER_EXECUTE(Domain, PinVcpu)
{
  virNodeInfo nodeinfo;
  int maxcpus;
  int cpumaplen;

  if(virNodeGetInfo(virDomainGetConnect(Handle().ToDomain()), &nodeinfo) == -1) {
    SetVirError(virGetLastError());
    return;
  }

  maxcpus = VIR_NODEINFO_MAXCPUS(nodeinfo);
  cpumaplen = VIR_CPU_MAPLEN(maxcpus);

  std::vector<unsigned char> cpumap(cpumaplen);

  for(int i = 0; i < vcpus_.size(); i++) {
    if(i > maxcpus)
      break;

    if(usables_[i])
      VIR_USE_CPU(cpumap.data(), vcpus_[i]);
    else
      VIR_UNUSE_CPU(cpumap.data(), vcpus_[i]);
  }

  if(virDomainPinVcpu(Handle().ToDomain(), vcpu_, cpumap.data(), cpumaplen) == -1) {
    SetVirError(virGetLastError());
    return;
  }

  data_ = true;
}

/////////////////////////////////////////////////////
// UNFINISHED - STILL SYNCHRONOUS
/////////////////////////////////////////////////////

NAN_METHOD(Domain::SetSchedulerParameters)
{
  NanScope();

  virSchedParameterPtr params = NULL;
  int nparams = 0;
  char *type = NULL;
  int ret = -1;

  if (args.Length() == 0 || !args[0]->IsObject()) {
    NanThrowTypeError("You must specify an object as argument to invoke this function");
    NanReturnUndefined();
  }

  Local<Object> newparams = args[0]->ToObject();
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

  type = virDomainGetSchedulerType(domain->handle_, &nparams);
  if (type == NULL) {
    ThrowException(Error::New(virGetLastError()));
    return NanFalse();
  }
  free(type);

  params = (virSchedParameterPtr) malloc(sizeof(*params) * nparams);
  if (params == NULL) {
    LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
    return NanFalse();
  }

  memset(params, 0, sizeof(*params) * nparams);

  ret = virDomainGetSchedulerParameters(domain->handle_, params, &nparams);
  if(ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    free(params);
    return NanFalse();
  }

  for (int i = 0; i < nparams; i++) {
    Local<String> field = NanNew(params[i].field);
    if (!newparams->Has(field)) {
      continue;
    }

    Local<Value> value = newparams->Get(field);

    switch (params[i].type) {
    case VIR_DOMAIN_SCHED_FIELD_INT:
      params[i].value.i = value->Int32Value();
      break;
    case VIR_DOMAIN_SCHED_FIELD_UINT:
      params[i].value.ui = value->Uint32Value();
      break;
    case VIR_DOMAIN_SCHED_FIELD_LLONG:
      params[i].value.l = value->NumberValue();
      break;
    case VIR_DOMAIN_SCHED_FIELD_ULLONG:
      params[i].value.ul = value->NumberValue();
      break;
    case VIR_DOMAIN_SCHED_FIELD_DOUBLE:
      params[i].value.d = value->NumberValue();
      break;
    case VIR_DOMAIN_SCHED_FIELD_BOOLEAN:
      params[i].value.b = value->Uint32Value();
      break;
    }
  }

  ret = virDomainSetSchedulerParameters(domain->handle_, params, nparams);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    free(params);
    return NanFalse();
  }
  free(params);

  return NanTrue();
}



NAN_METHOD(Domain::SetMigrationMaxDowntime)
{
  NLV_WARN_UNFINISHED(Domain::SetMigrationMaxDowntime);

  NanScope();
  long long downtime = 0;
  unsigned int flags = 0;
  int ret = -1;

  if (args.Length() == 0) {
    NanThrowTypeError("You must specify arguments to invoke this function");
    NanReturnUndefined();
  }

  if (!args[0]->IsInt32()) {
    NanThrowTypeError("You must specify a number as first argument");
    NanReturnUndefined();
  }

  downtime = args[0]->Int32Value();
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  ret = virDomainMigrateSetMaxDowntime(domain->handle_, downtime, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    return NanFalse();
  }

  return NanTrue();
}

NAN_METHOD(Domain::MemoryPeek)
{
  NLV_WARN_UNFINISHED(Domain::MemoryPeek);

  NanScope();

  unsigned long long start = 0;
  size_t size = 0;
  char * buffer_ = NULL;
  unsigned int flags = 0;

  if (args.Length() < 3) {
    NanThrowTypeError("You must specify three arguments to invoke this function");
    NanReturnUndefined();
  }

  if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
    NanThrowTypeError("You must specify a number in the first and second argument");
    NanReturnUndefined();
  }

  if (!args[2]->IsArray()) {
    NanThrowTypeError("You must specify an array in the third argument");
    NanReturnUndefined();
  }

  start = args[0]->NumberValue();
  size = args[1]->NumberValue() * sizeof(char *);

  //flags
  Local<Array> flags_ = args[2].As<Array>();
  unsigned int length = flags_->Length();
  for (unsigned int i = 0; i < length; i++) {
    flags |= flags_->Get(NanNew<Integer>(i))->Int32Value();
  }

  buffer_ = (char*) malloc(size);

  if (buffer_ == NULL) {
    LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
    return Null();
  }

  memset(buffer_, 0, size);

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

  int ret = virDomainMemoryPeek(domain->handle_, start, size, buffer_, flags);

  if (ret == -1) {
    free(buffer_);
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Buffer *buffer = Buffer::New(size);
  memcpy(Buffer::Data(buffer), buffer_, size);
  free(buffer_);

  NanReturnValue(buffer->handle_);
}

NAN_METHOD(Domain::BlockPeek)
{
  NLV_WARN_UNFINISHED(Domain::BlockPeek);

  NanScope();

  unsigned long long start = 0;
  size_t size = 0;
  char * buffer_ = NULL;
  unsigned int flags = 0;

  if (args.Length() < 3) {
    NanThrowTypeError("You must specify three arguments to invoke this function");
    NanReturnUndefined();
  }

  if (!args[0]->IsString()) {
    NanThrowTypeError("You must specify a string in the first and second argument");
    NanReturnUndefined();
  }

  if (!args[1]->IsNumber() || !args[2]->IsNumber()) {
    NanThrowTypeError("You must specify numbers in the first and second argument");
    NanReturnUndefined();
  }

  String::Utf8Value path(args[0]->ToString());

  start = args[1]->NumberValue();
  size = args[2]->NumberValue() * sizeof(char *);

  buffer_ = (char*) malloc(size);

  if (buffer_ == NULL) {
    LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
    NanReturnUndefined();
  }

  memset(buffer_, 0, size);

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

  int ret =
    virDomainBlockPeek(domain->handle_, (const char *) *path, start, size, buffer_, flags);
  if (ret == -1) {
    free(buffer_);
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Buffer *buffer = Buffer::New(size);
  memcpy(Buffer::Data(buffer), buffer_, size);
  free(buffer_);

  NanReturnValue(buffer->handle_);
}

NAN_METHOD(Domain::HasCurrentSnapshot)
{
  NLV_WARN_UNFINISHED(Domain::HasCurrentSnapshot);

  NanScope();
  unsigned int flags = 0;
  int ret = -1;

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

  ret = virDomainHasCurrentSnapshot(domain->handle_, flags);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    return NanFalse();
  }

  return ret == 1 ? NanTrue() : NanFalse();
}

NAN_METHOD(Domain::RevertToSnapshot)
{
  NLV_WARN_UNFINISHED(Domain::RevertToSnapshot);

  NanScope();
  virDomainSnapshotPtr snapshot = NULL;
  unsigned int flags = 0;
  int ret = -1;

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as argument to invoke this function");
    NanReturnUndefined();
  }

  std::string name(*NanUtf8String(args[0]->ToString()));
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  snapshot =
    virDomainSnapshotLookupByName(domain->handle_, name.c_str(), flags);
  if (snapshot == NULL) {
    ThrowException(Error::New(virGetLastError()));
    return NanFalse();
  }

  ret = virDomainRevertToSnapshot(snapshot, flags);
  if (ret == -1) {
    virDomainSnapshotFree(snapshot);
    ThrowException(Error::New(virGetLastError()));
    return NanFalse();
  }
  virDomainSnapshotFree(snapshot);

  return NanTrue();
}

NAN_METHOD(Domain::TakeSnapshot)
{
  NLV_WARN_UNFINISHED(Domain::TakeSnapshot);

  NanScope();
  virDomainSnapshotPtr snapshot = NULL;
  unsigned int flags = 0;

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

  if (args.Length() == 1 && args[0]->IsString()) {
    std::string xml(*NanUtf8String(args[0]->ToString()));
    snapshot = virDomainSnapshotCreateXML(domain->handle_, xml.c_str(), flags);
  } else {
    snapshot = virDomainSnapshotCurrent(domain->handle_, flags);
  }

  if (snapshot == NULL) {
    ThrowException(Error::New(virGetLastError()));
    return NanFalse();
  }

  virDomainSnapshotFree(snapshot);
  return NanTrue();
}

NAN_METHOD(Domain::GetCurrentSnapshot)
{
  NLV_WARN_UNFINISHED(Domain::GetCurrentSnapshot);

  NanScope();
  unsigned int flags = 0;
  virDomainSnapshotPtr snapshot = NULL;
  char* xml_ = NULL;

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  snapshot = virDomainSnapshotCurrent(domain->handle_, flags);
  if (snapshot == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  xml_ = virDomainSnapshotGetXMLDesc(snapshot, flags);
  if (xml_ == NULL) {
    virDomainSnapshotFree(snapshot);
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  virDomainSnapshotFree(snapshot);
  //TODO serialize to json

  Local<String> xml = NanNew(xml_);
  free(xml_);

  NanReturnValue(xml);
}

NAN_METHOD(Domain::DeleteSnapshot)
{
  NLV_WARN_UNFINISHED(Domain::DeleteSnapshot);

  NanScope();
  unsigned int flags = 0;
  virDomainSnapshotPtr snapshot = NULL;

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as argument to invoke this function");
    NanReturnUndefined();
  }


  std::string name(*NanUtf8String(args[0]->ToString()));
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  snapshot = virDomainSnapshotLookupByName(domain->handle_, name.c_str(), flags);
  if (snapshot == NULL) {
    ThrowException(Error::New(virGetLastError()));
    return NanFalse();
  }

  int ret = virDomainSnapshotDelete(snapshot, flags);
  if (ret == -1) {
    virDomainSnapshotFree(snapshot);
    ThrowException(Error::New(virGetLastError()));
    return NanFalse();
  }

  virDomainSnapshotFree(snapshot);
  return NanTrue();
}

NAN_METHOD(Domain::LookupSnapshotByName)
{
  NLV_WARN_UNFINISHED(Domain::lookupSnapshotByName);

  NanScope();
  char* xml_ = NULL;
  unsigned int flags = 0;
  virDomainSnapshotPtr snapshot = NULL;

  if (args.Length() == 0 || !args[0]->IsString()) {
    NanThrowTypeError("You must specify a string as argument to invoke this function");
    NanReturnUndefined();
  }


  std::string name(*NanUtf8String(args[0]->ToString()));
  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  snapshot = virDomainSnapshotLookupByName(domain->handle_, name.c_str(), flags);
  if (snapshot == NULL) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  xml_ = virDomainSnapshotGetXMLDesc(snapshot, flags);
  if (xml_ == NULL) {
    virDomainSnapshotFree(snapshot);
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  virDomainSnapshotFree(snapshot);
  //TODO serialize to json

  Local<String> xml = NanNew(xml_);
  free(xml_);

  NanReturnValue(xml);
}

NAN_METHOD(Domain::GetSnapshots)
{
  NLV_WARN_UNFINISHED(Domain::GetSnapshots);

  NanScope();
  char **snapshots_ = NULL;
  unsigned int flags = 0;
  int num_snapshots = 0;

  Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
  num_snapshots = virDomainSnapshotNum(domain->handle_, flags);
  if (num_snapshots == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  snapshots_ = (char**) malloc(sizeof(*snapshots_) * num_snapshots);
  if (snapshots_ == NULL) {
    LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
    NanReturnUndefined();
  }

  num_snapshots =
    virDomainSnapshotListNames(domain->handle_, snapshots_, num_snapshots, flags);
  if (num_snapshots == -1) {
    free(snapshots_);
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  Local<Array> snapshots = NanNew<Array>(num_snapshots);
  for (int i = 0; i < num_snapshots; i++) {
    virDomainSnapshotPtr snapshot =
      virDomainSnapshotLookupByName(domain->handle_, snapshots_[i], flags);
    char *xml = virDomainSnapshotGetXMLDesc(snapshot, flags);
    virDomainSnapshotFree(snapshot);

    snapshots->Set(Integer::New(i), String::New(xml));
    free(snapshots_[i]);
    free(xml);
  }
  free(snapshots_);

  NanReturnValue(snapshots);
}

NAN_METHOD(Domain::RegisterEvent)
{
  NLV_WARN_UNFINISHED(Domain::RegisterEvent);

  NanScope();
  if (args.Length() == 0 || !args[0]->IsObject()) {
    NanThrowTypeError("You must specify a object as argument");
    NanReturnUndefined();
  }

  Local<Object> arg_obj = args[0]->ToObject();
  if (!arg_obj->Has(NanNew("evtype")) ||
      !arg_obj->Get(NanNew("evtype"))->IsInt32()) {
    NanThrowTypeError("You must specify an valid event type");
    NanReturnUndefined();
  }

  if( !arg_obj->Has(NanNew("callback")) ||
      !arg_obj->Get(NanNew("callback"))->IsFunction()) {
    NanThrowTypeError("You must specify a valid callback function");
    NanReturnUndefined();
  }

  Domain *domain = NULL;
  if (arg_obj->Has(NanNew("domain"))) {
      Local<Object> domain_obj = arg_obj->Get(NanNew("domain"))->ToObject();
      if (!NanHasInstance(Domain::constructor_template, domain_obj)) {
        NanThrowTypeError("You must specify a Domain object instance");
        NanReturnUndefined();
      }

      domain = ObjectWrap::Unwrap<Domain>(domain_obj);
  }

  virConnectDomainEventGenericCallback callback = NULL;
  int evtype = arg_obj->Get(NanNew("evtype"))->Int32Value();

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

  Local<Value> jscallback = arg_obj->Get(NanNew("callback"));

  Persistent<Object> opaque = Persistent<Object>::New(NanNew<Object>());
  opaque->Set(NanNew("hypervisor"), args.This());
  opaque->Set(NanNew("callback"), jscallback);

  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
  int ret = virConnectDomainEventRegisterAny(hypervisor->handle_,
                                             domain != NULL ? domain->handle_ : NULL,
                                             evtype, callback,
                                             *opaque, domain_event_free);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnUndefined();
  }

  NanReturnValue(NanNew<Integer>(ret));
}

NAN_METHOD(Domain::UnregisterEvent)
{
  NLV_WARN_UNFINISHED(Domain::UnregisterEvent);

  NanScope();
  int ret = -1;
  int callback_id = 0;
  if (args.Length() == 0 || !args[0]->IsInt32()) {
    NanThrowTypeError("You must specify a integer as argument to call this function");
    NanReturnUndefined();
  }

  callback_id = args[0]->Int32Value();
  Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());
  ret = virConnectDomainEventDeregisterAny(hypervisor->handle_, callback_id);
  if (ret == -1) {
    ThrowException(Error::New(virGetLastError()));
    NanReturnValue(NanFalse());
  }

  NanReturnValue(NanTrue());
}

int Domain::domain_event_lifecycle_callback(virConnectPtr conn, virDomainPtr dom, int event,
                                            int detail, void *opaque)
{
  NanScope();
  Local<Object> domain_obj = Domain::NewInstance(dom);
  /*
    The virDomainPtr object handle passed into the callback upon delivery
    of an event is only valid for the duration of execution of the callback
    If the callback wishes to keep the domain object after the callback,
    it shall take a reference to it, by calling virDomainRef
  */
  virDomainRef(dom);

  Local<Value> argv[3];

  Persistent<Object> obj = static_cast<Object*>(opaque);
  Local<Object> hyp = obj->Get(NanNew("hypervisor"))->ToObject();

  Local<Function> callback = obj->Get(NanNew("callback")).As<Function>();
  Local<Object> data = NanNew<Object>();
  data->Set(NanNew("evtype"), NanNew<Integer>(event));
  data->Set(NanNew("detail"), NanNew<Integer>(detail));

  argv[0] = hyp;
  argv[1] = domain_obj; //FIXME change with domain->handle_
  argv[2] = data;

  TryCatch try_catch;
  callback->Call(hyp, 3, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  return 0;
}

int Domain::domain_event_generic_callback(virConnectPtr conn, virDomainPtr dom, void *opaque)
{
  NanScope();

  Local<Object> domain_obj = Domain::NewInstance(dom);
  /*
    The virDomainPtr object handle passed into the callback upon delivery
    of an event is only valid for the duration of execution of the callback
    If the callback wishes to keep the domain object after the callback,
    it shall take a reference to it, by calling virDomainRef
  */
  virDomainRef(dom);

  Local<Value> argv[2];
  Persistent<Object> obj = static_cast<Object*>(opaque);
  Local<Object> hyp = obj->Get(NanNew("hypervisor"))->ToObject();
  Local<Function> callback = obj->Get(NanNew("callback")).As<Function>();

  argv[0] = hyp;
  argv[1] = domain_obj; //FIXME change with domain->handle_
  TryCatch try_catch;
  callback->Call(hyp, 2, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  return 0;
}

int Domain::domain_event_rtcchange_callback(virConnectPtr conn, virDomainPtr dom,
                                            long long utcoffset, void *opaque)
{
  NanScope();

  Local<Object> domain_obj = Domain::NewInstance(dom);

  /*
    The virDomainPtr object handle passed into the callback upon delivery
    of an event is only valid for the duration of execution of the callback
    If the callback wishes to keep the domain object after the callback,
    it shall take a reference to it, by calling virDomainRef
  */
  virDomainRef(dom);

  Local<Value> argv[3];
  Persistent<Object> obj = static_cast<Object*>(opaque);
  Local<Object> hyp = obj->Get(NanNew("hypervisor"))->ToObject();

  Local<Function> callback = obj->Get(NanNew("callback")).As<Function>();
  Local<Object> data = NanNew<Object>();
  data->Set(NanNew("utc_offset"), NanNew<Number>(utcoffset));

  argv[0] = hyp;
  argv[1] = domain_obj; //FIXME change with domain->handle_
  argv[2] = data;

  TryCatch try_catch;
  callback->Call(hyp, 3, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  return 0;
}

int Domain::domain_event_watchdog_callback(virConnectPtr conn, virDomainPtr dom, int action,
                                           void *opaque)
{
  NanScope();

  Local<Object> domain_obj = Domain::NewInstance(dom);

  /*
    The virDomainPtr object handle passed into the callback upon delivery
    of an event is only valid for the duration of execution of the callback
    If the callback wishes to keep the domain object after the callback,
    it shall take a reference to it, by calling virDomainRef
  */
  virDomainRef(dom);

  Local<Value> argv[3];
  Persistent<Object> obj = static_cast<Object*>(opaque);
  Local<Object> hyp = obj->Get(NanNew("hypervisor"))->ToObject();
  Local<Function> callback = obj->Get(NanNew("callback")).As<Function>();

  Local<Object> data = NanNew<Object>();
  data->Set(NanNew("action"), NanNew<Integer>(action));

  argv[0] = hyp;
  argv[1] = domain_obj; //FIXME change with domain->handle_
  argv[2] = data;

  TryCatch try_catch;
  callback->Call(hyp, 3, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  return 0;
}

int Domain::domain_event_io_error_callback(virConnectPtr conn, virDomainPtr dom,
                                           const char *src_path, const char *dev_alias,
                                           int action, void *opaque)
{
  NanScope();

  Local<Object> domain_obj = Domain::NewInstance(dom);

  /*
    The virDomainPtr object handle passed into the callback upon delivery
    of an event is only valid for the duration of execution of the callback
    If the callback wishes to keep the domain object after the callback,
    it shall take a reference to it, by calling virDomainRef
  */
  virDomainRef(dom);

  Local<Value> argv[3];
  Persistent<Object> obj = static_cast<Object*>(opaque);
  Local<Object> hyp = obj->Get(NanNew("hypervisor"))->ToObject();
  Local<Function> callback = obj->Get(NanNew("callback")).As<Function>();

  Local<Object> data = NanNew<Object>();
  data->Set(NanNew("src_path"), NanNew(src_path));
  data->Set(NanNew("dev_alias"), NanNew(dev_alias));
  data->Set(NanNew("action"), NanNew<Integer>(action));

  argv[0] = hyp;
  argv[1] = domain_obj; //FIXME change with domain->handle_
  argv[2] = data;

  TryCatch try_catch;
  callback->Call(hyp, 3, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  return 0;
}

int Domain::domain_event_io_error_reason_callback(virConnectPtr conn, virDomainPtr dom,
                                                  const char *src_path, const char *dev_alias,
                                                  int action, const char *reason, void *opaque)
{
  NanScope();
  Local<Object> domain_obj = Domain::NewInstance(dom);

  /*The virDomainPtr object handle passed into the callback upon delivery
  of an event is only valid for the duration of execution of the callback
  If the callback wishes to keep the domain object after the callback,
  it shall take a reference to it, by calling virDomainRef*/
  virDomainRef(dom);

  Local<Value> argv[3];
  Persistent<Object> obj = static_cast<Object*>(opaque);
  Local<Object> hyp = obj->Get(NanNew("hypervisor"))->ToObject();
  Local<Function> callback = obj->Get(NanNew("callback")).As<Function>();

  Local<Object> data = NanNew<Object>();
  data->Set(NanNew("src_path"), NanNew(src_path));
  data->Set(NanNew("dev_alias"), NanNew(dev_alias));
  data->Set(NanNew("reason"), NanNew(reason));
  data->Set(NanNew("action"), NanNew<Integer>(action));

  argv[0] = hyp;
  argv[1] = domain_obj; //FIXME change with domain->handle_
  argv[2] = data;

  TryCatch try_catch;
  callback->Call(hyp, 3, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  return 0;
}

int Domain::domain_event_graphics_callback(virConnectPtr conn, virDomainPtr dom, int phase,
                                           virDomainEventGraphicsAddressPtr local,
                                           virDomainEventGraphicsAddressPtr remote,
                                           const char *auth_scheme,
                                           virDomainEventGraphicsSubjectPtr subject,
                                           void *opaque)
{
  NanScope();
  Local<Object> domain_obj = Domain::NewInstance(dom);

  /*
    The virDomainPtr object handle passed into the callback upon delivery
    of an event is only valid for the duration of execution of the callback
    If the callback wishes to keep the domain object after the callback,
    it shall take a reference to it, by calling virDomainRef
  */
  virDomainRef(dom);

  Local<Value> argv[3];
  Persistent<Object> obj = static_cast<Object*>(opaque);
  Local<Object> hyp = obj->Get(NanNew("hypervisor"))->ToObject();
  Local<Function> callback = obj->Get(NanNew("callback")).As<Function>();

  Local<String> lfamily;
  switch (local->family) {
  case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV4:
    lfamily = NanNew("ipv4");
    break;
  case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV6:
    lfamily = NanNew("ipv6");
    break;
  };

  Local<String> rfamily;
  switch(remote->family) {
  case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV4:
    rfamily = NanNew("ipv4");
    break;
  case VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV6:
    rfamily = NanNew("ipv6");
    break;
  };

  Local<Object> local_ = NanNew<Object>();
  local_->Set(NanNew("family"), lfamily);
  local_->Set(NanNew("node"), NanNew(local->node));
  local_->Set(NanNew("service"), NanNew(local->service));

  Local<Object> remote_ = NanNew<Object>();
  remote_->Set(NanNew("family"), rfamily);
  remote_->Set(NanNew("node"), NanNew(remote->node));
  remote_->Set(NanNew("service"), NanNew(remote->service));

  int nidentity = subject->nidentity;
  Local<Array> subject_ = NanNew<Array>(nidentity);
  for (int i = 0; i < nidentity; i++) {
    Local<Object> identity = NanNew<Object>();
    identity->Set(NanNew("type"), NanNew(subject->identities[i].type));
    identity->Set(NanNew("name"), NanNew(subject->identities[i].name));
    subject_->Set(NanNew<Integer>(i), identity);
  }

  Local<Object> data = NanNew<Object>();
  data->Set(NanNew("local"), local_);
  data->Set(NanNew("remote"), remote_);
  data->Set(NanNew("subject"), subject_);
  data->Set(NanNew("phase"), NanNew<Integer>(phase));
  data->Set(NanNew("auth_scheme"), NanNew(auth_scheme));

  argv[0] = hyp;
  argv[1] = domain_obj; //FIXME change with domain->handle_
  argv[2] = data;

  TryCatch try_catch;
  callback->Call(hyp, 3, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }
  return 0;
}

void Domain::domain_event_free(void* /* opaque */)
{
  fprintf(stderr, "NOT IMPLEMENTED!");
}




} //namespace NodeLibvirt
