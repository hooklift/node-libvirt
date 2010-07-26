# Node-Libvirt API

## Hypervisor
* ### hypervisor.getBaselineCPU(xmlCpus)
Computes the most feature-rich CPU which is compatible with all given host CPUs.
#### Params
 Array of XML descriptions of host CPUs, you can specify two or more cpu descriptions.
#### Return
 An XML description of the computed CPU or null plus an exception explaining
 the error.
#### Example
        var cpu1 = 'xml description1';
        var cpu2 = 'xml description2';

        var xmlCPUs = [cpu1, cpu2];

        //no supported by test driver
        try {
            var baseline = hypervisor.getBaselineCPU(xmlCPUs);
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }

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

