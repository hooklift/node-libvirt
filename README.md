node-libvirt - The virtualization API bindings for [NodeJS][nodejs_home]
=============

[![Build Status](https://secure.travis-ci.org/hooklift/node-libvirt.svg?branch=master)](https://travis-ci.org/hooklift/node-libvirt)
[![Dependency Status](https://david-dm.org/hooklift/node-libvirt.svg)](https://david-dm.org/hooklift/node-libvirt)
[![npm version](https://badge.fury.io/js/libvirt.svg)](http://badge.fury.io/js/libvirt)
[![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/hooklift/node-libvirt?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Bountysource](https://www.bountysource.com/badge/tracker?tracker_id=339079)](https://www.bountysource.com/trackers/339079-node-libvirt?utm_source=339079&utm_medium=shield&utm_campaign=TRACKER_BADGE)

Libvirt is a toolkit to interact with virtualization capabilities
of recent versions of GNU/Linux (and other OSes). (reference: [Libvirt][libvirt_home])

Node-Libvirt is a set of bindings to Libvirt API, which allows to use it from Javascript.

### Virtualization Technologies Supported

   * The [Xen][xen_home] hypervisor on Linux and Solaris hosts.
   * [QEMU][qemu_home] emulator
   * [KVM][kvm_home] Linux hypervisor
   * [LXC][lxc_home] Linux container system
   * [OpenVZ][openvz_home] Linux container system
   * [User Mode Linux][user_mode_linux_home] paravirtualized kernel
   * [VirtualBox][virtualbox_home] hypervisor
   * [VMware ESX and GSX][vmware_home] hypervisors
   * [IBM Power][phyp_home] Hypervisor


### Capabilities

   * Management of virtual machines, virtual networks and storage
   * Remote management using TLS encryption and x509 certificates
   * Remote management authenticating with Kerberos and SASL
   * Local access control using PolicyKit
   * Zero-conf discovery using Avahi multicast-DNS
   * Support for storage on IDE/SCSI/USB disks, FibreChannel, LVM, iSCSI, NFS and filesystems

## Installation
You must have [NodeJS][nodejs_dev] and [Libvirt][libvirt_dev] already installed to be able to build node-libvirt.

### Get and Install node-libvirt
    $ sudo apt-get install libvirt-dev
    $ git clone git://github.com/hooklift/node-libvirt.git
    $ cd node-libvirt
    $ npm install OR ( node-gyp configure && node-gyp build )

## Example of use
For now please take a look at the tests.

## API
Work in progress at [docs page][docs_link]

## License
(The MIT License)

Copyright 2015 node-libvirt Authors. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

[nodejs_home]: http://www.nodejs.org
[nodejs_dev]: http://github.com/ry/node
[libvirt_home]: http://www.libvirt.org
[libvirt_dev]: http://libvirt.org/deployment.html
[xen_home]: http://www.cl.cam.ac.uk/Research/SRG/netos/xen/index.html
[qemu_home]: http://wiki.qemu.org/Index.html
[kvm_home]: http://www.linux-kvm.org
[lxc_home]: http://lxc.sourceforge.net/
[openvz_home]: http://openvz.org/
[user_mode_linux_home]: http://user-mode-linux.sourceforge.net/
[virtualbox_home]: http://www.virtualbox.org/
[vmware_home]: http://www.vmware.com/
[wiki_examples]: http://github.com/c4milo/node-libvirt/wikis/examples
[wiki_api]: http://github.com/c4milo/node-libvirt/wikis/api
[docs_link]: http://github.com/c4milo/node-libvirt/blob/master/API.md
[phyp_home]: http://www.ibm.com/developerworks/wikis/display/LinuxP/POWER5+Hypervisor

