#include <libvirt/libvirt.h>
#include "libvirt_handle.h"

namespace NodeLibvirt {



struct LibVirtHandlePrivate
{
  LibVirtHandlePrivate() {
    data.connection = 0;
    data.interface = 0;
    data.network = 0;
  }

  union Data {
    virConnectPtr connection;
    virInterfacePtr interface;
    virNetworkPtr network;
  } data;
};

LibVirtHandle::LibVirtHandle()
  : d(new LibVirtHandlePrivate)
{
}

LibVirtHandle::LibVirtHandle(virConnectPtr connection)
  : d(new LibVirtHandlePrivate)
{
  d->data.connection = connection;
}

LibVirtHandle::LibVirtHandle(virInterfacePtr interface)
  : d(new LibVirtHandlePrivate)
{
  d->data.interface = interface;
}

LibVirtHandle::LibVirtHandle(virNetworkPtr network)
  : d(new LibVirtHandlePrivate)
{
  d->data.network = network;
}

LibVirtHandle::~LibVirtHandle()
{
}

void LibVirtHandle::Clear()
{
  d->data.connection = 0;

  if (d->data.interface)
    virInterfaceFree(d->data.interface);
  d->data.interface = 0;

  if (d->data.network)
    virNetworkFree(d->data.network);
  d->data.network = 0;
}

virConnectPtr LibVirtHandle::ToConnection() const
{
  return d->data.connection;
}

virInterfacePtr LibVirtHandle::ToInterface() const
{
  return d->data.interface;
}

virNetworkPtr LibVirtHandle::ToNetwork() const
{
  return d->data.network;
}

} // namespace NodeLibvirt
