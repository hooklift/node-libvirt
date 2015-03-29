// Copyright 2013, Camilo Aguilar. Cloudescape, LLC.

#include <nan.h>

#include "hypervisor.h"
#include "error.h"
#include "node_device.h"
#include "interface.h"
#include "network.h"
#include "network_filter.h"
#include "secret.h"
#include "storage_pool.h"
#include "storage_volume.h"
#include "node_libvirt.h"

using namespace v8;
using namespace NodeLibvirt;

void InitAll(Handle<Object> exports)
{
  virInitialize();
  Hypervisor::Initialize(exports);
  Error::Initialize();
  // Domain::Initialize();
  NodeDevice::Initialize();
  Interface::Initialize();
  Network::Initialize();
  NetworkFilter::Initialize();
  Secret::Initialize();
  StoragePool::Initialize();
  StorageVolume::Initialize();

  exports->Set(NanNew("version"), NanNew(NODE_LIBVIRT_VERSION));
  exports->Set(NanNew("libvirt_version"), NanNew(LIBVIR_VERSION_NUMBER));
}

NODE_MODULE(libvirt, InitAll)

