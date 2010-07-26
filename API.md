# Node-Libvirt API

## Hypervisor

### hypervisor.getBaselineCPU([cpu1, cpu2, cpu3, cpuN])
 Computes the most feature-rich CPU which is compatible with all given host CPUs.
#### Parameters:
 Array of XML descriptions of host CPUs.
#### Return:
 An XML description of the computed CPU or null plus an exception with an Error instance.

### hypervisor.compareCPU(cpu)
 Compares the given CPU description with the host CPU
#### Parameters:
 An xml string with the cpu description
#### Return:
 One of the following constants:
 libvirt.VIR_CPU_COMPARE_INCOMPATIBLE,
 libvirt.VIR_CPU_COMPARE_IDENTICAL,
 libvirt.VIR_CPU_COMPARE_SUPERSET. Or null plus an exception with an Error
 instance if ocurred an error.

### hypervisor.getCapabilities()
 Provides a xml with capabilities of the hypervisor.
#### Parameters:
 None.
#### Return:
 An xml describing capabilities of the current hypervisor.

### hypervisor.getHostname()
 This returns the system hostname on which the hypervisor is running
 (the result of the gethostname system call). If we are connected to
 a remote system, then this returns the hostname of the remote system.
#### Parameters:
 None.
#### Return:
The system hostname or null plus an exception with an Error instance

### hypervisor.getLibVirtVersion()
Provides the libvirt version used by the daemon running on the hypervisor host.
#### Parameters:
 None.
#### Return:
An libvirt version string with format: major.minor.release. i.e. 1.8.2. If there is
an error, it returns null plus an exception with an Error instance.

### hypervisor.getMaxVcpus(type)
Provides the maximum number of virtual CPUs supported for a guest VM of a
specific type. The `type` parameter here corresponds to the `type` attribute
in the `<domain>` element of the XML. The `type` values are driver specific,
but include "xen", "kvm", "qemu", "lxc" and "kqemu".
#### Parameters:
 A string specifying the driver type
#### Return:
A number which represents the maximum number of virtual CPUs supported or null
plus an exception with an Error instance.

### hypervisor.getType()
Provides the name of the Hypervisor software used.
#### Parameters:
None.
#### Return:
An string which represents the Hypervisor software used or null plus an exception
with an Error instance.

### hypervisor.GetConnectionUri()
Provides the URI (name) of the hypervisor connection. Normally this is the same
as or similar to the string passed to the `new Hypervisor()`, but the driver
may make the URI canonical.
#### Parameters:
None.
#### Return
An hypervisor URI string or null plus an exception with an Error instance.

### hypervisor.getVersion()
Get the version level of the Hypervisor running. This may work only with
hypervisor call, i.e. with privileged access to the hypervisor, not with a
read-only connection.
#### Parameters:
None.
#### Return:
An hypervisor version string with format: major.minor.release. i.e. 0.12.3 If
there is an error, it returns null plus an exception with an Error instance.

### hypervisor.isConnectionEncrypted()
Determine if the connection to the hypervisor is encrypted
#### Parameters:
None.
#### Return:
true or false plus an exception with an Error instance if an error ocurred.

### hypervisor.isConnectionSecure()
Determine if the connection to the hypervisor is secure.
The connection will be classed as secure if it is either encrypted, or running
over a channel which is not exposed to eavesdropping (eg a UNIX domain socket, or pipe).
#### Parameters:
None.
#### Return:
true or false plus an exception with an Error instance if an error ocurred.

### hypervisor.closeConnection()
Close the hypervisor connection. This should not be called if further interaction
with the Hypervisor are needed especially if there is running domain which
need further monitoring by the application.
#### Parameters:
None.
#### Return:
true or false plus an exception with an Error instance if an error ocurred.

### hypervisor.getDefinedDomains()
### hypervisor.getDefinedInterfaces()
### hypervisor.getDefinedNetworks()
### hypervisor.getDefinedStoragePools()
### hypervisor.getActiveDomains()
### hypervisor.getActiveInterfaces()
### hypervisor.getNetworkFilters()
### hypervisor.getActiveNetworks()
### hypervisor.getSecrets()
### hypervisor.getActiveStoragePools()
### hypervisor.getNumberOfDefinedDomains()
### hypervisor.getNumberOfDefinedInterfaces()
### hypervisor.getNumberOfDefinedNetworks()
### hypervisor.getNumberOfDefinedStoragePools()
### hypervisor.getNumberOfActiveDomains()
### hypervisor.getNumberOfActiveInterfaces()
### hypervisor.getNumberOfActiveNetworks()
### hypervisor.getNumberOfNetworkFilters()
### hypervisor.getNumberOfSecrets()
### hypervisor.getNumberOfActiveStoragePools()
### hypervisor.createDomain()
### hypervisor.defineDomain()
### hypervisor.restoreDomain()
### hypervisor.lookupDomainById()
### hypervisor.lookupDomainByName()
### hypervisor.lookupDomainByUUID()
### hypervisor.getNodeFreeMemoryInNumaCells()
### hypervisor.getNodeFreeMemory()
### hypervisor.getNodeInfo()
### hypervisor.getNodeDevicesNames()
### hypervisor.getNodeSecurityModel()
### hypervisor.lookupNodeDeviceByName()
### hypervisor.createNodeDevice()
### hypervisor.createNetwork()
### hypervisor.lookupNetworkByName()
### hypervisor.lookupNetworkByUUID()
### hypervisor.defineNetwork()
### hypervisor.defineNetworkFilter()
### hypervisor.lookupNetworkFilterByName()
### hypervisor.lookupNetworkFilterByUUID()
### hypervisor.lookupInterfaceByName()
### hypervisor.lookupInterfaceByMacAddress()
### hypervisor.defineInterface()
### hypervisor.defineSecret()
### hypervisor.lookupSecretByUsage()
### hypervisor.lookupSecretByUUID()
### hypervisor.createStoragePool()
### hypervisor.defineStoragePool()
### hypervisor.lookupStoragePoolByName()
### hypervisor.lookupStoragePoolByUUID()
### hypervisor.lookupStoragePoolByVolume()
### hypervisor.lookupStorageVolumeByKey()
### hypervisor.lookupStorageVolumeByPath()
### hypervisor.registerDomainEvent()
### hypervisor.unregisterDomainEvent()

## Domain
## Network
## NetworkFilter
## Interface
## StoragePool
## StorageVolume
## NodeDevice
## Error
## Event

