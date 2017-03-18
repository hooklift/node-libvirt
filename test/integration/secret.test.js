'use strict';

var libvirt = require('../../lib'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('../lib/helper').fixture,
    expect = require('chai').expect;

var test = {
  // helper while developing
  removeSecret: function(uuid) {
    return test.hypervisor.lookupSecretByUUIDAsync(uuid)
      .then(function(secret) { return secret.undefineAsync(); });
  }
};

describe('Secret (integration)', function() {
  before(function() {
    SegfaultHandler.registerHandler();
  });

  describe('hypervisor methods', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('qemu:///system');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;
        done();
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        done();
      });
    });

    it('should be located through its uuid', function()  {
      return test.hypervisor.defineSecretAsync(fixture('secret.xml'))
        .then(function(secret) { return secret.getUUIDAsync(); })
        .then(function(uuid) { return test.hypervisor.lookupSecretByUUIDAsync(uuid); })
        .then(function(secret) {
          expect(secret).to.exist;
          return secret.undefineAsync();
        });
    });

    it('should support setValue/getValue (#89)', function() {
      var data = 'some_test_value', secret;
      return test.hypervisor.defineSecretAsync(fixture('secret2.xml'))
        .then(function(secret_) { secret = secret_; })
        .then(function() { return secret.setValueAsync(data); })
        .then(function() { return secret.getValueAsync(); })
        .then(function(result) { expect(result).to.eql(data); })
        .finally(function() { return secret.undefineAsync(); });
    });
  });

});
