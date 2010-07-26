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

### hypervisor.getConnectionUri()
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
These four functions return the names of **all** the defined: Domains,
Interfaces, Networks and StoragePools respectively.

#### Parameters:
None.
#### Return:
An array of strings which are the names of Domains, Interfaces, Networks
or StoragePools currently defined but not active on the hypervisor. It also
return null plus an exception if some error ocurred.

### hypervisor.getNumberOfDefinedDomains()
### hypervisor.getNumberOfDefinedInterfaces()
### hypervisor.getNumberOfDefinedNetworks()
### hypervisor.getNumberOfDefinedStoragePools()
These four functions return the number of **all** currently defined but not
active: Domains, Interfaces, Networks and StoragePools respectively.

#### Parameters:
None.
#### Return:
A number which represents the total of currently defined: Domains,
Interfaces, Networks and StoragePools respectively. It also
return null plus an exception if some error ocurred.

### hypervisor.getActiveDomains()
### hypervisor.getActiveInterfaces()
### hypervisor.getActiveNetworks()
### hypervisor.getActiveStoragePools()
These four functions return the names of **all** currently active: Domains,
Interfaces, Networks or StoragePools respectively.

#### Parameters:
None.
#### Return:
An array of strings which are the names of the Domains, Interfaces, Networks,
or StoragePools currently active on the hypervisor. It also
return null plus an exception if some error ocurred.

### hypervisor.getNumberOfActiveDomains()
### hypervisor.getNumberOfActiveInterfaces()
### hypervisor.getNumberOfActiveNetworks()
### hypervisor.getNumberOfActiveStoragePools()
These four functions return the number of **all** currently active: Domains,
Interfaces, Networks and StoragePools respectively.

#### Parameters:
None.
#### Return:
A number which represents the total of currently active: Domains,
Interfaces, Networks and StoragePools respectively. It also
return null plus an exception if some error ocurred.

### hypervisor.getNetworkFilters()
Provides the names of all the network filters.

#### Parameters:
None.
#### Return:
An array of strings with the names of the network filters or null plus
an exception if some error ocurred.

### hypervisor.getNumberOfNetworkFilters()
Provides the number of network filters.

#### Parameters:
None.
#### Return:
A number which represents the number of network filters or null plus
an exception if some error ocurred.

### hypervisor.getSecrets()
List UUIDs of defined secrets

#### Parameters:
None.
#### Return:
An array of strings with the UUIDs of the secrets or null plus
an exception if some error ocurred

### hypervisor.getNumberOfSecrets()
Fetch number of currently defined secrets.

#### Parameters:
None.
#### Return:
A number which represents the total of currently defined secrets or null
plus an exception if some error ocurred

### hypervisor.createDomain(xml)
Launch a new guest domain, based on an XML description. This function may
requires privileged access to the hypervisor. The domain is not persistent,
so its definition will disappear when it is stopped, or if the host is
restarted.

#### Parameters:
An string representing the Xml domain description
#### Return:
An instance of Domain or null if some error ocurred during the creation.

### hypervisor.defineDomain(xml)
Define a domain, but does not start it. This definition is persistent, until
explicitly undefined with `domain.undefine()`. A previous definition for this
domain would be overriden if it already exists.

#### Parameters:
An string representing the Xml domain description.
#### Return:
An instance of Domain or null if some error ocurred during the creation.

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

