'use strict';

var libvirt = require('../build/Release/libvirt'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

var test = {};
describe('Interface', function() {
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

    it('should define the interface from its xml description', function() {
      var xml = fixture('interface.xml');
      var iface = test.hypervisor.defineInterface(xml);
      expect(iface.getName()).to.equal('eth0');
    });

    it('should undefine the interface', function() {
      var iface = test.hypervisor.lookupInterfaceByName('eth0');
      expect(iface.undefine()).to.be.ok;
    });

    it('should be located through its name', function() {
      var iface = test.hypervisor.lookupInterfaceByName('eth1');
      expect(iface.getName()).to.equal('eth1');
    });

    it('should be located through its mac address', function() {
      var iface = test.hypervisor.lookupInterfaceByMacAddress('aa:bb:cc:dd:ee:ff');
      expect(iface.getName()).to.equal('eth1');
    });
  });

  describe('methods', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;
        test.interface = test.hypervisor.lookupInterfaceByName('eth1');
        done();
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        test.interface = undefined;
        done();
      });
    });

    it('should stop', function() {
      expect(test.interface.stop()).to.be.ok;
    });

    it('should start', function() {
      expect(test.interface.start()).to.be.ok;
    });

    it('should indicate if is active and running', function() {
      expect(test.interface.isActive()).to.be.true;
    });

    it('should return the name', function() {
      expect(test.interface.getName()).to.equal('eth1');
    });

    it('should return the mac address', function() {
      expect(test.interface.getMacAddress()).to.equal('aa:bb:cc:dd:ee:ff');
    });

    it('should return its xml description', function() {
      expect(test.interface.toXml([])).to.match(/eth1/);
    });
  });
});
