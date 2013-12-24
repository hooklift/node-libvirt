# Node-Libvirt API

## Hypervisor

### new Hypervisor(uri, options)
 Options:

 - user: (optional) username for authentication
 - password: (optional) password for authentication
 - readonly: (optional) whether the connection is read-only

 If options is a boolean it is used as the readonly parameter

### getBaselineCPU([cpu1, cpu2, cpu3, cpuN])
 Computes the most feature-rich CPU which is compatible with all given host CPUs.
#### Parameters:
 Array of XML descriptions of host CPUs.
#### Return:
 An XML description of the computed CPU or null plus an exception with an Error instance.

### compareCPU(cpu)
 Compares the given CPU description with the host CPU
#### Parameters:
 An xml string with the cpu description
#### Return:
 One of the following constants:
 libvirt.VIR_CPU_COMPARE_INCOMPATIBLE,
 libvirt.VIR_CPU_COMPARE_IDENTICAL,
 libvirt.VIR_CPU_COMPARE_SUPERSET. Or null plus an exception with an Error
 instance if occurred an error.

### getCapabilities()
 Provides a xml with capabilities of the hypervisor.
#### Parameters:
 None.
#### Return:
 An xml describing capabilities of the current hypervisor.

### getHostname()
 This returns the system hostname on which the hypervisor is running
 (the result of the gethostname system call). If we are connected to
 a remote system, then this returns the hostname of the remote system.
#### Parameters:
 None.
#### Return:
The system hostname or null plus an exception with an Error instance

### getLibVirtVersion()
Provides the libvirt version used by the daemon running on the hypervisor host.
#### Parameters:
 None.
#### Return:
An libvirt version string with format: major.minor.release. i.e. 1.8.2. If there is
an error, it returns null plus an exception with an Error instance.

### getMaxVcpus(type)
Provides the maximum number of virtual CPUs supported for a guest VM of a
specific type. The `type` parameter here corresponds to the `type` attribute
in the `<domain>` element of the XML. The `type` values are driver specific,
but include "xen", "kvm", "qemu", "lxc" and "kqemu".
#### Parameters:
 A string specifying the driver type
#### Return:
A number which represents the maximum number of virtual CPUs supported or null
plus an exception with an Error instance.

### getType()
Provides the name of the Hypervisor software used.
#### Parameters:
None.
#### Return:
A string which represents the Hypervisor software used or null plus an exception
with an Error instance.

### getConnectionUri()
Provides the URI (name) of the hypervisor connection. Normally this is the same
as or similar to the string passed to the `new Hypervisor()`, but the driver
may make the URI canonical.
#### Parameters:
None.
#### Return
An hypervisor URI string or null plus an exception with an Error instance.

### getVersion()
Get the version level of the Hypervisor running. This may work only with
hypervisor call, i.e. with privileged access to the hypervisor, not with a
read-only connection.
#### Parameters:
None.
#### Return:
An hypervisor version string with format: major.minor.release. i.e. 0.12.3 If
there is an error, it returns null plus an exception with an Error instance.

### isConnectionEncrypted()
Determine if the connection to the hypervisor is encrypted
#### Parameters:
None.
#### Return:
true or false plus an exception with an Error instance if an error occurred.

### isConnectionSecure()
Determine if the connection to the hypervisor is secure.
The connection will be classed as secure if it is either encrypted, or running
over a channel which is not exposed to eavesdropping (eg a UNIX domain socket, or pipe).
#### Parameters:
None.
#### Return:
true or false plus an exception with an Error instance if an error occurred.

### closeConnection()
Close the hypervisor connection. This should not be called if further interaction
with the Hypervisor are needed especially if there is running domain which
need further monitoring by the application.
#### Parameters:
None.
#### Return:
true or false plus an exception with an Error instance if an error occurred.

### getDefinedDomains()
### getDefinedInterfaces()
### getDefinedNetworks()
### getDefinedStoragePools()
These four functions return the names of **all** the defined: Domains,
Interfaces, Networks and StoragePools respectively.

#### Parameters:
None.
#### Return:
An array of strings which are the names of Domains, Interfaces, Networks
or StoragePools currently defined but not active on the hypervisor. It also
return null plus an exception if some error occurred.

### getNumberOfDefinedDomains()
### getNumberOfDefinedInterfaces()
### getNumberOfDefinedNetworks()
### getNumberOfDefinedStoragePools()
These four functions return the number of **all** currently defined but not
active: Domains, Interfaces, Networks and StoragePools respectively.

#### Parameters:
None.
#### Return:
A number which represents the total of currently defined: Domains,
Interfaces, Networks and StoragePools respectively. It also
return null plus an exception if some error occurred.

### getActiveDomains()
### getActiveInterfaces()
### getActiveNetworks()
### getActiveStoragePools()
These four functions return the names of **all** currently active: Domains,
Interfaces, Networks or StoragePools respectively.

#### Parameters:
None.
#### Return:
An array of strings which are the names of the Domains, Interfaces, Networks,
or StoragePools currently active on the hypervisor. It also
return null plus an exception if some error occurred.

### getNumberOfActiveDomains()
### getNumberOfActiveInterfaces()
### getNumberOfActiveNetworks()
### getNumberOfActiveStoragePools()
These four functions return the number of **all** currently active: Domains,
Interfaces, Networks and StoragePools respectively.

#### Parameters:
None.
#### Return:
A number which represents the total of currently active: Domains,
Interfaces, Networks and StoragePools respectively. It also
return null plus an exception if some error occurred.

### getNetworkFilters()
Provides the names of all the network filters.

#### Parameters:
None.
#### Return:
An array of strings with the names of the network filters or null plus
an exception if some error occurred.

### getNumberOfNetworkFilters()
Provides the number of network filters.

#### Parameters:
None.
#### Return:
A number which represents the number of network filters or null plus
an exception if some error occurred.

### getSecrets()
List UUIDs of defined secrets

#### Parameters:
None.
#### Return:
An array of strings with the UUIDs of the secrets or null plus
an exception if some error occurred

### getNumberOfSecrets()
Fetch number of currently defined secrets.

#### Parameters:
None.
#### Return:
A number which represents the total of currently defined secrets or null
plus an exception if some error occurred

### createDomain(xml)
Launch a new guest domain, based on an XML description. This function may
requires privileged access to the hypervisor. The domain is not persistent,
so its definition will disappear when it is stopped, or if the host is
restarted.

#### Parameters:
A string representing the Xml domain description
#### Return:
An instance of Domain or null if some error occurred during the creation.

### defineDomain(xml)
Define a domain, but does not start it. This definition is persistent, until
explicitly undefined with `domain.undefine()`. A previous definition for this
domain would be overriden if it already exists.

#### Parameters:
A string representing the Xml domain description.
#### Return:
An instance of Domain or null if some error occurred during the creation.

### restoreDomain(path)
Restore a domain saved to disk by `save()` from the `path` specified.

#### Parameters:
The path where the domain was saved.
#### Return:
true or false plus an exception with an Error instance.

### lookupDomainById(id)
Try to find a domain based on the hypervisor ID number. Note that this won't
work for inactive domains which have an ID of -1, in that case a lookup based
on the Name or UUId need to be done instead.

#### Parameters:
A id number
#### Return:
A Domain instance or null plus an exception if some error occurred.

### lookupDomainByName(name)
Try to lookup a domain based on its name.

#### Parameters:
A string with the domain name to lookup.
#### Return:
A Domain instance or null plus an exception if some error occurred.

### lookupDomainByUUID(uuid)
Try to lookup a domain based on its UUID.

#### Parameters:
A string with the domain uuid to lookup.
#### Return:
A Domain instance or null plus an exception if some error occurred.

### getNodeFreeMemoryInNumaCells(startCell, maxCell)
This function returns the amount of free memory in one or more NUMA cells.

#### Parameters:
* **startCell**: Index of first cell.
* **maxCell**: Maximum number of cells.

#### Return:
An array of cells with its amount of free memory or null plus an exception if
some error occurred.

### getNodeFreeMemory()
Provides the free memory available on the Node. Most libvirt APIs provide memory
sizes in kilobytes, but in this function the returned value is in bytes.
Divide by 1024 as necessary.

#### Parameters:
None.
#### Return:
A number represeting free memory in bytes or null plus an exception if some error
occurred.

### getNodeInfo()
Extract hardware information about the node.

#### Parameters:
None.
#### Return:
An object with the following structure:
    { 'model':      'myCpuModel', //string indicating the CPU model
      'memory':     1024,         //memory size in kilobytes
      'cpus':       4,            //the number of active CPUs
      'mhz':        2000,         //expected CPU frecuency
      'nodes':      2,            //the number of NUMA cell, 1 for uniform mem access
      'sockets':    4,            //number of CPU socket per node
      'cores':      4,            //number of core per socket
      'threads':    16            //number of threads per core
    }
It also returns null plus an exception if some error occurred.

### getNodeDevicesNames([capXml])
Provides the names of node devices. If the optional `capXml` argument is specified,
then it will return devices with the specified capability.

#### Parameters:
An optional string with the xml describing device capabilites. It works like a
device filter.

#### Return:
An array of strings with the devices names.

### getNodeSecurityModel()
Extract the security model of a hypervisor.

#### Parameters:
None.
#### Return:
An object with the following structure:
    { 'model':  'selinux',
      'doi':    0
    }

### lookupNodeDeviceByName(name)
Lookup a node device by its name.

#### Parameters:
A string with the name to lookup
#### Return:
An instance of NodeDevice or null plus an exception if some error occurred.

### createNodeDevice(xml)
Create a new device on the VM host machine, for example, virtual HBAs created
using vport_create.

#### Parameters:
A string with the device xml description.
#### Return:
An instance of NodeDevice or null plus an exception if some error occurred.

### createNetwork(xml)
Create and start a new virtual network.

#### Parameters:
A string with the network xml description.

#### Return:
An instance of Network or null plus an exception if some error occurred.

### lookupNetworkByName(name)
Try to lookup a network based on its name.

#### Parameters:
A string with the network name to lookup.
#### Return:
An instance of Network or null plus an exception if some error occurred.

### lookupNetworkByUUID(uuid)
Try to lookup a network based on its UUID.

#### Parameters:
A string with the network uuid to lookup.
#### Return:
An instance of Network or null plus an exception if some error occurred.

### defineNetwork(xml)
Define a network, but it does not start it.

#### Parameters:
A string with the network xml description.
#### Return:
An instance of Network or null plus an exception if some error occurred.

### defineNetworkFilter(xml)
Define a new network filter, based on an XML description.

#### Parameters:
A xml with the filter xml description.
#### Return
An insntace of NetworkFilter or null plus an exception if some error occurred.

### lookupNetworkFilterByName(name)
Try to lookup a network filter based on its name.

#### Parameters:
A string with the filter name to lookup

#### Return:
An instance of NetworkFilter or null plus an exception if some error occurred.

### lookupNetworkFilterByUUID(uuid)
Try to lookup a network filter based on its UUID.

#### Parameters:
An string with the filter uuid to lookup
#### Return:
An instance of NetworkFilter or null plus an exception if some error occurred.

### lookupInterfaceByName(name)
Try to lookup an interface based on its name.

#### Parameters
A string with the interface name to look for the interface.
#### Return:
An instance of Interface or null plus an exception if some error occurred.

### lookupInterfaceByMacAddress(mac)
Try to lookup an interface on the given hypervisor based on its MAC Address.

#### Parameters:
A string with the interface mac address to look for the interface

#### Return:
An instance of Interface or null plus an exception if some error occurred.

### defineInterface(xml)
Define an interface (or modify existing interface configuration).

#### Parameters:
A string with the interface xml description.
#### Return:
An instance of Interface or null plus an exception if some error occurred.

### defineSecret(xml)
If XML specifies a UUID, locates the specified secret and replaces all
attributes of the secret specified by UUID by attributes specified in
xml (any attributes not specified in xml are discarded). Otherwise,
creates a new secret with an automatically chosen UUID, and initializes
its attributes from xml.

#### Parameters:
An string with the secret xml description.
#### Return:
An instance of Secret or null plus an exception if some error occurred.

### lookupSecretByUsage(usageType, usageId)
Try to lookup a secret based on its usage. The usageId is unique within the set
of secrets sharing the same usageType value.

#### Parameters:
* **usageType**: String with the type of secret usage.
* **usageId**: Number with the identifier of the object using the secret.

#### Return:
An instance of Secret or null plus an exception if some error occurred.

### lookupSecretByUUID(uuid)
Try to lookup a secret based on its UUID.

#### Parameters:
An string with the secret uuid.
#### Return:
An instance of Secret or null plus an exception if some error occurred.

### createStoragePool(xml)
Create a new storage based on its XML description. The pool is not persistent,
so its definition will disappear when it is stopped, or if the host is restarted.

#### Parameters:
A string with the pool xml description.
#### Return:
An instance of StoragePool or null plus an exception if some error occurred.

### defineStoragePool(xml)
Define a new inactive storage pool based on its XML description.
The pool is persistent, until explicitly undefined.

#### Parameters:
A string with the pool xml description.
#### Return:
An instance of StoragePool or null plus an exception if some error occurred.

### lookupStoragePoolByName(name)
Fetch a storage pool based on its unique name.

#### Parameters:
A string with the pool name.
#### Return:
An instance of StoragePool or null plus an exception if some error occurred.

### lookupStoragePoolByUUID(uuid)
Fetch a storage pool based on its globally unique id

#### Parameters:
A string with the pool uuid.
#### Return:
An instance of StoragePool or null plus an exception if some error occurred.

### lookupStoragePoolByVolume(volume) (unimplemented)
Fetch a storage pool which contains a particular volume

#### Parameters:
An instance of StorageVolume
#### Return:
An instance of StoragePool or null plus an exception if some error occurred.

### lookupStorageVolumeByKey(key)
Fetch a pointer to a storage volume based on its globally unique key.

#### Parameters:
A string with the volume key.
#### Return:
An instance of StorageVolume or null plus an exception if some error occurred.

### lookupStorageVolumeByPath(path)
Fetch a pointer to a storage volume based on its locally (host) unique path.

#### Parameters:
A string with the volume path.
#### Return:
An instance of StorageVolume or null plus an exception if some error occurred.

### registerDomainEvent(event)
Adds a callback to receive notifications of arbitrary domain events occurring
on a domain. If property `domain` is not present, then events will be monitored
for any domain. If property `domain` is present, then only the specific
domain will be monitored Most types of event have a callback providing a
custom set of parameters for the event.

#### Parameters:
An object with the following structure:
    { 'evtype':     hypervisor.VIR_DOMAIN_EVENT_ID_LIFECYCLE,
      'domain':     mydomain,
      'callback':   function(hyp, dom, data) {

                     }
    }
evtype is an OR'ed set of the following constants:
    hypervisor.VIR_DOMAIN_EVENT_ID_LIFECYCLE
    hypervisor.VIR_DOMAIN_EVENT_ID_REBOOT
    hypervisor.VIR_DOMAIN_EVENT_ID_RTC_CHANGE
    hypervisor.VIR_DOMAIN_EVENT_ID_WATCHDOG
    hypervisor.VIR_DOMAIN_EVENT_ID_IO_ERROR
    hypervisor.VIR_DOMAIN_EVENT_ID_GRAPHICS
    hypervisor.VIR_DOMAIN_EVENT_ID_IO_ERROR_REASON
    hypervisor.VIR_DOMAIN_EVENT_ID_LAST

#### Return:


### unregisterDomainEvent()
Removes an event callback. The callbackID parameter should be the vaule obtained
from a previous `registerDomainEvent(event)` function.

## Domain
## Network
## NetworkFilter
## Interface
## StoragePool
## StorageVolume
## NodeDevice
## Error
## Event

