'use strict';

var libvirt = require('../lib'),
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

    it('should define and undefine an interface from its xml description', function(done) {
      var xml = fixture('interface.xml');
      test.hypervisor.defineInterface(xml, function(err, iface) {
        expect(err).to.not.exist;
        expect(iface).to.exist;
        expect(iface._parent).to.exist;

        iface.getName(function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.equal('eth2');
          done();
        });
      });
    });

    it('should undefine the interface', function(done) {
      var xml = fixture('interface.xml');
      test.hypervisor.defineInterface(xml, function(err, iface) {
        expect(err).to.not.exist;
        expect(iface).to.exist;
        expect(iface._parent).to.exist;

        test.hypervisor.lookupInterfaceByName('eth2', function(err, iface) {
          expect(err).to.not.exist;
          expect(iface).to.exist;
          expect(iface._parent).to.exist;
          iface.undefine(function(err, result) {
            expect(err).to.not.exist;
            expect(result).to.be.true;
            done();
          });
        });
      });
    });

    it('should be located through its name', function(done) {
      test.hypervisor.lookupInterfaceByName('eth1', function(err, iface) {
        expect(err).to.not.exist;
        expect(iface).to.exist;
        expect(iface._parent).to.exist;

        iface.getName(function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.equal('eth1');
          done();
        });
      });
    });

    it('should be located through its mac address', function(done) {
      test.hypervisor.lookupInterfaceByMacAddress('aa:bb:cc:dd:ee:ff', function(err, iface) {
        expect(err).to.not.exist;
        expect(iface).to.exist;
        expect(iface._parent).to.exist;

        iface.getName(function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.equal('eth1');
          done();
        });
      });
    });
  });

  describe('methods', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;
        test.hypervisor.lookupInterfaceByName('eth1', function(err, iface) {
          expect(err).to.not.exist;
          expect(iface).to.exist;
          expect(iface._parent).to.exist;
          test.interface = iface;
          done();
        });
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        test.interface = undefined;
        done();
      });
    });

    it('should start', function(done) {
      test.interface.start(function(err, result) {
        expect(err).to.exist;

        // NOTE: not supported in test-driver, so result is false
        expect(result).to.not.exist;
        done();
      });
    });

    it('should stop', function(done) {
      // NOTE: not supported in test-driver, so result is false
      test.interface.stop(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.false;
        done();
      });
    });

    it('should indicate if is active and running', function(done) {
      test.interface.isActive(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;
        done();
      });
    });

    it('should return the name', function(done) {
      test.interface.getName(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.equal('eth1');
        done();
      });
    });

    it('should return the mac address', function(done) {
      test.interface.getMacAddress(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.equal('aa:bb:cc:dd:ee:ff');
        done();
      });
    });

    it('should return its xml description', function(done) {
      test.interface.toXml(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.match(/eth1/);
        done();
      });
    });
  });
});
