// Copyright 2013, Camilo Aguilar. Cloudescape, LLC.

#include <nan.h>

#include "event_impl.h"
#include "domain.h"
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
using namespace NLV;

void InitAll(Handle<Object> exports)
{
  virInitialize();
  Hypervisor::Initialize(exports);
  Error::Initialize(exports);
  Domain::Initialize(exports);
  NodeDevice::Initialize(exports);
  Interface::Initialize(exports);
  Network::Initialize(exports);
  NetworkFilter::Initialize(exports);
  Secret::Initialize(exports);
  StoragePool::Initialize(exports);
  StorageVolume::Initialize(exports);
  EventImpl::Initialize(exports);

  exports->Set(Nan::New("version").ToLocalChecked(), Nan::New(NODE_LIBVIRT_VERSION).ToLocalChecked());
  exports->Set(Nan::New("libvirt_version").ToLocalChecked(), Nan::New(LIBVIR_VERSION_NUMBER));
}

NODE_MODULE(libvirt, InitAll)
