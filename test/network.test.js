'use strict';

var libvirt = require('../build/Release/libvirt'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

var test = {};
describe('Network', function() {
  before(function() {
    SegfaultHandler.registerHandler();
  });

  describe('hypervisor methods', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
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

    it('should create and start an already defined network', function() {
      var xml = fixture('network.xml');
      var net = test.hypervisor.defineNetwork(xml);
      expect(net.start()).to.be.ok;
    });

    it('should create and start a new virtual network from its xml description', function() {
      var xml = fixture('network.xml');
      var net = test.hypervisor.createNetwork(xml);
      expect(net.getName()).to.equal('test');
    });

    it('should lookup the network by name', function() {
      var net = test.hypervisor.lookupNetworkByName('default');
      expect(net.getName()).to.equal('default');
    });

    it('should define a network from its xml description', function() {
      var xml = fixture('network.xml');
      var net = test.hypervisor.defineNetwork(xml);
      expect(net.getName()).to.equal('test');
    });
  });

  describe('methods', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;
        test.network = test.hypervisor.lookupNetworkByName('default');
        done();
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        test.network = undefined;
        done();
      });
    });

    it('should determine if the network has a persistent configuration', function() {
      expect(test.network.isPersistent()).to.be.true;
    });

    it('should determine if the network is currently running', function() {
      expect(test.network.isActive()).to.be.true;
    });

    it('should provide a xml network description', function() {
      expect(test.network.toXml([])).to.match(/<name>default<\/name>/);
    });

    it('should return the network uuid', function() {
      expect(test.network.getUUID()).to.be.ok;
    });

    it('should return the network name', function() {
      expect(test.network.getName()).to.equal('default');
    });

    it('should return the bridge interface name', function() {
      expect(test.network.getBridgeName()).to.equal('virbr0');
    });

    it('should indicate if the network is configured to be automatically started when the host boots', function() {
      expect(test.network.getAutostart()).to.be.false;
    });

    it('should configure the network to be automatically started when the host boots', function() {
      expect(test.network.setAutostart(false)).to.be.ok;
      /*
      NOTE: doesn't work with test driver
      expect(test.network.getAutostart()).to.be.false;

      expect(test.network.setAutostart(true)).to.be.ok;
      expect(test.network.getAutostart()).to.be.true;
      */
    });

    it('should lookup the network by uuid', function() {
      var uuid = test.network.getUUID();
      var net = test.hypervisor.lookupNetworkByUUID(uuid);
      expect(net.getName()).to.equal('default');
    });

    it('should undefine a network', function() {
      var net = test.hypervisor.lookupNetworkByName('test');
      expect(net.destroy()).to.be.ok;
      expect(function() { net.undefine(); }).to.throw(Error);
    });

    it('should destroy a network', function() {
      expect(test.network.destroy()).to.be.ok;
    });
  });
});
