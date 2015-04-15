'use strict';

var libvirt = require('bindings')('libvirt'),
    Promise = require("bluebird");

Promise.promisifyAll(libvirt.Hypervisor.prototype);
Promise.promisifyAll(libvirt.Domain.prototype);
Promise.promisifyAll(libvirt.NodeDevice.prototype);
Promise.promisifyAll(libvirt.Interface.prototype);
Promise.promisifyAll(libvirt.Network.prototype);
Promise.promisifyAll(libvirt.NetworkFilter.prototype);
Promise.promisifyAll(libvirt.Secret.prototype);
Promise.promisifyAll(libvirt.StoragePool.prototype);
Promise.promisifyAll(libvirt.StorageVolume.prototype);

module.exports = {
  Promise: Promise,
  createHypervisor: function(uri, options) {
    return new libvirt.Hypervisor(uri, options);
  }
};
