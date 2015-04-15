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

/*
 * A helper method returning an 'all domains' promise.
 */
libvirt.Hypervisor.prototype.getAllDomains = function() {
  var defined;

  return this.listDefinedDomainsAsync()
  .map(function(name) {
    return this.lookupDomainByNameAsync(name);
  }.bind(this))
  .then(function(domains) {
    defined = domains;
    return this.listActiveDomainsAsync();
  }.bind(this))
  .map(function(id) {
    return this.lookupDomainByIdAsync(id);
  }.bind(this))
  .then(function(domains) {
    return domains.concat(defined);
  })
}

module.exports = {
  Promise: Promise,
  startEventLoop: libvirt.setupEvent,
  createHypervisor: function(uri, options) {
    return new libvirt.Hypervisor(uri, options);
  }
};
