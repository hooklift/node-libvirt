#ifndef LIBVIRT_HANDLE_H
#define LIBVIRT_HANDLE_H

#include <libvirt/libvirt.h>

#ifdef __APPLE__
# include <tr1/memory>
using namespace std::tr1;
#else
# include <memory>
#endif

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
  LibVirtHandle(virDomainPtr domain);

  void Clear();

  virConnectPtr ToConnection() const;
  virInterfacePtr ToInterface() const;
  virNetworkPtr ToNetwork() const;
  virNWFilterPtr ToNetworkFilter() const;
  virStoragePoolPtr ToStoragePool() const;
  virStorageVolPtr ToStorageVolume() const;
  virNodeDevicePtr ToNodeDevice() const;
  virSecretPtr ToSecret() const;
  virDomainPtr ToDomain() const;

private:
  shared_ptr<LibVirtHandlePrivate> d;

};

} // namespace NodeLibvirt

#endif  // LIBVIRT_HANDLE_H
