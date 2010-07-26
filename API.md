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
The libvir version with format: major.minor.release. Ex. 1.8.2. If there is
an error, it returns null plus an exception with an Error instance.

### hypervisor.getMaxVcpus(type)
Provides the maximum number of virtual CPUs supported for a guest VM of a
specific type. The `type` parameter here corresponds to the `type` attribute
in the `<domain>` element of the XML. The `type` values are driver specific,
but include "xen", "kvm", "qemu", "lxc" and "kqemu".
#### Parameters:
 And string specifying the driver type
#### Return:
A number which represents the maximum number of virtual CPUs supported.

## Domain
## Network
## NetworkFilter
## Interface
## StoragePool
## StorageVolume
## NodeDevice
## Error
## Event

