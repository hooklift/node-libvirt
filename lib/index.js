'use strict';

var libvirt = require('bindings')('libvirt'),
    Promise = require("bluebird"),
    EventEmitter = require('events').EventEmitter;

// extend prototype
function inherits(target, source) {
    for (var k in source.prototype) {
        target.prototype[k] = source.prototype[k];
    }
}

Promise.promisifyAll(libvirt.Hypervisor.prototype);
Promise.promisifyAll(libvirt.Domain.prototype);
Promise.promisifyAll(libvirt.NodeDevice.prototype);
Promise.promisifyAll(libvirt.Interface.prototype);
Promise.promisifyAll(libvirt.Network.prototype);
Promise.promisifyAll(libvirt.NetworkFilter.prototype);
Promise.promisifyAll(libvirt.Secret.prototype);
Promise.promisifyAll(libvirt.StoragePool.prototype);
Promise.promisifyAll(libvirt.StorageVolume.prototype);

inherits(libvirt.Domain, EventEmitter);

/*
 * A helper method returning an 'all domains' promise.
 */
libvirt.Hypervisor.prototype.getAllDomains = function() {
  var defined;

  return this.listDefinedDomainsAsync().bind(this)
  .map(function(name) {
    return this.lookupDomainByNameAsync(name);
  })
  .then(function(domains) {
    defined = domains;
    return this.listActiveDomainsAsync();
  })
  .map(function(id) {
    return this.lookupDomainByIdAsync(id);
  })
  .then(function(domains) {
    return domains.concat(defined);
  });
};

libvirt.Promise = Promise;
libvirt.startEventLoop = libvirt.setupEvent,
libvirt.createHypervisor = function(uri, options) {
  return new libvirt.Hypervisor(uri, options);
};

module.exports = libvirt;

// module.exports = {
//   libvirt: libvirt,
//   Promise: Promise,
//   startEventLoop: libvirt.setupEvent,
//   createHypervisor: function(uri, options) {
//     return new libvirt.Hypervisor(uri, options);
//   }
// };
