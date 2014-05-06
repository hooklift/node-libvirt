'use strict';

var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var libvirt = require('./build/Release/libvirt');

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('esx://172.16.103.128/?no_verify=1', {
    username: 'root',
    password: 'shadow04101',
    readOnly: false
});

console.log(hypervisor.getVersion());
console.log(hypervisor.getCapabilities());
