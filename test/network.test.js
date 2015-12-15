'use strict';

var libvirt = require('../lib'),
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

    it('should create and start an already defined network', function(done) {
      var xml = fixture('network.xml');
      test.hypervisor.defineNetwork(xml, function(err, network) {
        expect(err).to.not.exist;
        expect(network).to.exist;
        expect(network._parent).to.exist;

        network.start(function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.be.true;
          done();
        });
      });
    });

    it('should create and start a new virtual network from its xml description', function(done) {
      var xml = fixture('network.xml');
      test.hypervisor.createNetwork(xml, function(err, network) {
        expect(err).to.not.exist;
        expect(network).to.exist;
        expect(network._parent).to.exist;

        network.getName(function(err, name) {
          expect(err).to.not.exist;
          expect(name).to.exist;
          expect(name).to.equal('test');
          done();
        });
      });
    });

    it('should lookup the network by name', function(done) {
      test.hypervisor.lookupNetworkByName('default', function(err, network) {
        expect(err).to.not.exist;
        expect(network).to.exist;
        expect(network._parent).to.exist;

        network.getName(function(err, name) {
          expect(err).to.not.exist;
          expect(name).to.exist;
          expect(name).to.equal('default');
          done();
        });
      });
    });

    it('should define a network from its xml description', function(done) {
      var xml = fixture('network.xml');
      test.hypervisor.defineNetwork(xml, function(err, network) {
        expect(err).to.not.exist;
        expect(network).to.exist;
        expect(network._parent).to.exist;

        network.getName(function(err, name) {
          expect(err).to.not.exist;
          expect(name).to.exist;
          expect(name).to.equal('test');
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

        test.hypervisor.lookupNetworkByName('default', function(err, network) {
          expect(err).to.not.exist;
          expect(network).to.exist;
          expect(network._parent).to.exist;
          test.network = network;
          done();
        });
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        test.network = undefined;
        done();
      });
    });

    it('should determine if the network has a persistent configuration', function(done) {
      test.network.isPersistent(function(err, persistent) {
        expect(err).to.not.exist;
        expect(persistent).to.be.true;
        done();
      });
    });

    it('should determine if the network is currently running', function(done) {
      test.network.isActive(function(err, active) {
        expect(err).to.not.exist;
        expect(active).to.be.true;
        done();
      });
    });

    it('should provide a xml network description', function(done) {
      test.network.toXml(function(err, data) {
        expect(err).to.not.exist;
        expect(data).to.match(/<name>default<\/name>/);
        done();
      });
    });

    it('should return the network uuid', function(done) {
      test.network.getUUID(function(err, uuid) {
        expect(err).to.not.exist;
        expect(uuid).to.exist;
        done();
      });
    });

    it('should return the network name', function(done) {
      test.network.getName(function(err, name) {
        expect(err).to.not.exist;
        expect(name).to.exist;
        expect(name).to.equal('default');
        done();
      });
    });

    it('should return the bridge interface name', function(done) {
      test.network.getBridgeName(function(err, name) {
        expect(err).to.not.exist;
        expect(name).to.equal('virbr0');
        done();
      });
    });

    it('should indicate if the network is configured to be automatically started when the host boots', function(done) {
      test.network.getAutostart(function(err, autostart) {
        expect(err).to.not.exist;
        expect(autostart).to.exist;
        expect(autostart).to.be.false;
        done();
      });
    });

    it('should configure the network to be automatically started when the host boots', function(done) {
      test.network.setAutostart(false, function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;
        done();
      });

     /*
      NOTE: doesn't work with test driver
      expect(test.network.getAutostart()).to.be.false;

      expect(test.network.setAutostart(true)).to.be.ok;
      expect(test.network.getAutostart()).to.be.true;
      */
    });

    it('should lookup the network by uuid', function(done) {
      test.network.getUUID(function(err, uuid) {
        expect(err).to.not.exist;
        expect(uuid).to.exist;

        test.hypervisor.lookupNetworkByUUID(uuid, function(err, network) {
          expect(err).to.not.exist;
          expect(network).to.exist;
          expect(network._parent).to.exist;

          network.getName(function(err, name) {
            expect(err).to.not.exist;
            expect(name).to.exist;
            expect(name).to.equal('default');
            done();
          });
        });
      });
    });

    it('should undefine a network', function(done) {
      var xml = fixture('network.xml');
      test.hypervisor.defineNetwork(xml, function(err, network) {
        expect(err).to.not.exist;
        expect(network).to.exist;
        expect(network._parent).to.exist;

        test.hypervisor.lookupNetworkByName('test', function(err, network) {
          expect(err).to.not.exist;
          expect(network).to.exist;
          expect(network._parent).to.exist;

          network.destroy(function(err, result) {
            expect(err).to.not.exist;
            expect(result).to.be.true;

            network.undefine(function(err, result) {
              expect(err).to.not.exist;
              expect(result).to.be.true;
              done();
            });
          });
        });
      });
    });

    it('should destroy a network', function(done) {
      test.network.destroy(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;
        done();
      });
    });
  });
});
