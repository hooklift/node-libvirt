'use strict';

var libvirt = require('bindings')('libvirt'),
    Promise = require("bluebird"),
    EventEmitter = require('events').EventEmitter,
    util = require('util');

function LibvirtError(message) {
  Error.captureStackTrace(this, this.constructor);
  this.name = this.constructor.name;
  this.message = message;
}

util.inherits(LibvirtError, Error);

function errorHandler(err) {
  var newError = new LibvirtError(err.message);
  for (var key in err) newError[key] = err[key];
  throw newError;
}

function promisifyMethod(module, method) {
  var promisifed = Promise.promisify(module[method]);
  module[method] = function() {
    var len = arguments.length;
    var args = new Array(len);
    for (var i = 0; i < len; ++i) args[i] = arguments[i];

    var nodeback;
    if (args.length && typeof args[args.length - 1] === 'function') {
      nodeback = args.pop();
    }

    var result = promisifed.apply(this, args);
    if (!!nodeback) return result.asCallback(nodeback);
    return result.catch(errorHandler);
  };
}

function promisifyModule(module) {
  for (var method in module)
    promisifyMethod(module, method);
}

promisifyModule(libvirt.Hypervisor.prototype);
promisifyModule(libvirt.Hypervisor.prototype);
promisifyModule(libvirt.Domain.prototype);
promisifyModule(libvirt.NodeDevice.prototype);
promisifyModule(libvirt.Interface.prototype);
promisifyModule(libvirt.Network.prototype);
promisifyModule(libvirt.NetworkFilter.prototype);
promisifyModule(libvirt.Secret.prototype);
promisifyModule(libvirt.StoragePool.prototype);
promisifyModule(libvirt.StorageVolume.prototype);

// form of inherits that only extends the prototype, safe for native module
function inherits(target, source) {
  for (var k in source.prototype) {
    target.prototype[k] = source.prototype[k];
  }
}

inherits(libvirt.Domain, EventEmitter);

/*
 * A helper method returning an 'all domains' promise.
 */
libvirt.Hypervisor.prototype.getAllDomains = function() {
  var self = this;
  return Promise.join([ this.listDefinedDomains(), this.listActiveDomains() ])
    .spread(function(defined, active) { return defined.concat(active); })
    .spread(function(defined, active) {
      return Promise.all([
        Promise.map(defined, function(domain) { return self.lookupDomainByName(domain); }),
        Promise.map(active, function(domain) { return self.lookupDomainById(domain); })
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
