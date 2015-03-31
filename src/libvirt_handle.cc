#include <libvirt/libvirt.h>
#include "libvirt_handle.h"

namespace NodeLibvirt {

struct LibVirtHandlePrivate
{
  LibVirtHandlePrivate() {
    data.connection = 0;
    data.interface = 0;
    data.network = 0;
    data.networkFilter = 0;
  }

  ~LibVirtHandlePrivate() {
    Clear();
  }

  void Clear() {
    data.connection = 0;

    if (data.interface)
      virInterfaceFree(data.interface);
    data.interface = 0;

    if (data.network)
      virNetworkFree(data.network);
    data.network = 0;

    if (data.networkFilter)
      virNWFilterFree(data.networkFilter);
    data.networkFilter = 0;
  }

  union Data {
    virConnectPtr connection;
    virInterfacePtr interface;
    virNetworkPtr network;
    virNWFilterPtr networkFilter;
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

LibVirtHandle::LibVirtHandle(virNWFilterPtr networkFilter)
  : d(new LibVirtHandlePrivate)
{
  d->data.networkFilter = networkFilter;
}

void LibVirtHandle::Clear()
{
  d->Clear();
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

virNWFilterPtr LibVirtHandle::ToNetworkFilter() const
{
  return d->data.networkFilter;
}


} // namespace NodeLibvirt
