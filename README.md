# Node-libvirt - The virtualization API bindings for [NodeJS][nodejs_home]
Libvirt is a toolkit to interact with the virtualization capabilities
of recent versions of GNU/Linux (and other OSes). (reference: [Libvirt][libvirt_home])

Node-Libvirt is a set of bindings to Libvirt C API, which allows to use the API from Javascript

## Installation
You must have [NodeJS][nodejs_dev] and [Libvirt][libvirt_dev] already installed to be able to build node-libvirt.

    $ git clone git://github.com/c4milo/node-libvirt.git
    $ cd node-libvirt
    $ node-waf configure build
    $ node specs.js

## Features
### Virtualization Technologies Supported
   * The Xen hypervisor on Linux and Solaris hosts.
   * QEMU
   * KVM
   * LXC
   * OpenVZ
   * User Mode Linux
   * VirtualBox
   * VMware ESX and GSX



   * Storage on IDE/SCSI/USB disks, FibreChannel, LVM, iSCSI, NFS and filesystems



## License
(The MIT License)

Copyright 2010 Camilo Aguilar. All rights reserved.

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

