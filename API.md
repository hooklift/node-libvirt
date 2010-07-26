# Node-Libvirt API

## Hypervisor

### hypervisor.getBaselineCPU([cpu1, cpu2, cpu3, ...])
 Computes the most feature-rich CPU which is compatible with all given host CPUs.
#### Parameters:
 Array of XML descriptions of host CPUs.
#### Return:
 An XML description of the computed CPU or null plus an exception with an Error instance.

* ### hypervisor.getBaselineCPU(xmlCpus)
Computes the most feature-rich CPU which is compatible with all given host CPUs
#### Params
 Array of XML descriptions of host CPUs
#### Return
#### Example
## Domain
## Network
## NetworkFilter
## Interface
## StoragePool
## StorageVolume
## NodeDevice
## Error
## Event

