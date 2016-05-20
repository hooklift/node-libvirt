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

function LibvirtError(message) {
  Error.captureStackTrace(this, this.constructor);
  this.name = this.constructor.name;
  this.message = message;
}

require('util').inherits(LibvirtError, Error);

function errorHandler(err) {
  var newError = new LibvirtError(err.message);
  for(var key in err) {
    newError[key] = err[key];
  }
  throw newError;
}

var promisifyOptions = {
  promisifier: function(originalFunction, defaultPromisifer) {
    var promisified = defaultPromisifer(originalFunction);
    return function() {
      return promisified.apply(this, arguments)
        .catch(errorHandler);
    };
  }
};

Promise.promisifyAll(libvirt.Hypervisor.prototype, promisifyOptions);
Promise.promisifyAll(libvirt.Domain.prototype, promisifyOptions);
Promise.promisifyAll(libvirt.NodeDevice.prototype, promisifyOptions);
Promise.promisifyAll(libvirt.Interface.prototype, promisifyOptions);
Promise.promisifyAll(libvirt.Network.prototype, promisifyOptions);
Promise.promisifyAll(libvirt.NetworkFilter.prototype, promisifyOptions);
Promise.promisifyAll(libvirt.Secret.prototype, promisifyOptions);
Promise.promisifyAll(libvirt.StoragePool.prototype, promisifyOptions);
Promise.promisifyAll(libvirt.StorageVolume.prototype, promisifyOptions);

inherits(libvirt.Domain, EventEmitter);

/*
 * A helper method returning an 'all domains' promise.
 */
libvirt.Hypervisor.prototype.getAllDomains = function() {
  var self = this;
  return Promise.join([ this.listDefinedDomainsAsync(), this.listActiveDomainsAsync() ])
    .spread(function(defined, active) { return defined.concat(active); })
    .spread(function(defined, active) {
      return Promise.all([
        Promise.map(defined, function(domain) { return self.lookupDomainByNameAsync(domain); }),
        Promise.map(active, function(domain) { return self.lookupDomainByIdAsync(domain); })
      ]);
    })
    .spread(function(defined, active) { return defined.concat(active); });
};

libvirt.Promise = Promise;
libvirt.startEventLoop = libvirt.setupEvent,
libvirt.createHypervisor = function(uri, options) {
  return new libvirt.Hypervisor(uri, options);
};

module.exports = libvirt;
module.exports.LibvirtError = LibvirtError;
