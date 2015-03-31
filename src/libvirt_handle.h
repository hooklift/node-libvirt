#ifndef LIBVIRT_HANDLE_H
#define LIBVIRT_HANDLE_H

#include <libvirt/libvirt.h>
#include <memory>
using namespace std;

namespace NodeLibvirt {

class LibVirtHandlePrivate;
class LibVirtHandle
{
public:
  LibVirtHandle();
  LibVirtHandle(virConnectPtr connection);
  LibVirtHandle(virInterfacePtr interface);
  LibVirtHandle(virNetworkPtr network);
  LibVirtHandle(virNWFilterPtr networkFilter);
  LibVirtHandle(virStoragePoolPtr storagePool);
  LibVirtHandle(virStorageVolPtr storageVolume);
  LibVirtHandle(virNodeDevicePtr nodeDevice);
  LibVirtHandle(virSecretPtr secret);

  void Clear();

  virConnectPtr ToConnection() const;
  virInterfacePtr ToInterface() const;
  virNetworkPtr ToNetwork() const;
  virNWFilterPtr ToNetworkFilter() const;
  virStoragePoolPtr ToStoragePool() const;
  virStorageVolPtr ToStorageVolume() const;
  virNodeDevicePtr ToNodeDevice() const;
  virSecretPtr ToSecret() const;

private:
  shared_ptr<LibVirtHandlePrivate> d;

};

} // namespace NodeLibvirt

#endif  // LIBVIRT_HANDLE_H
