'use strict';

var libvirt = require('../build/Release/libvirt'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

//TODO create a Node class and add detach attach
var test = {};
describe('Node Device', function() {
  before(function() {
    SegfaultHandler.registerHandler();
  });

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

  describe('hypervisor methods', function() {
    it('should create a node device using its xml description', function() {
      //test driver does not provide mechanisms to test this function
      try {
        var xml = fixture('node_device.xml');
        var device = test.hypervisor.createNodeDevice(xml);
        expect(device).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_INTERNAL_ERROR);
      }
    });

    it('should lookup a node device by name', function() {
      //test driver does not provide mechanisms to test this function
      try {
        var devices = test.hypervisor.getNodeDevicesNames();
        var device = test.hypervisor.lookupNodeDeviceByName(devices[0]);
        expect(device).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });
  });

  describe('methods', function() {
    beforeEach(function() {
      try {
        var xml = fixture('node_device.xml');
        test.device = test.hypervisor.createNodeDevice(xml);
        expect(test.device).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_INTERNAL_ERROR);
      }
    });

    afterEach(function() {
      test.device = undefined;
    });

    it('should detach the device from the node itself in order to be assigned to a guest domain', function() {
      //test driver does not provide mechanisms to test this function
      try {
        expect(test.device.detach()).to.be.ok;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should reattach a previously detached node device', function() {
      //test driver does not provide mechanisms to test this function
      try {
        expect(test.device.reattach()).to.be.ok;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should reset the node device a previously detached node device', function() {
      //test driver does not provide mechanisms to test this function
      try {
        expect(test.device.reset()).to.be.ok;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should return the device name', function() {
      //test driver does not provide mechanisms to test this function
      try {
        var name = test.device.getName();
        expect(name).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should return the device parent name', function() {
      //test driver does not provide mechanisms to test this function
      try {
        var parent = test.device.getParentName();
        expect(parent).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should return de device xml description', function() {
      //test driver does not provide mechanisms to test this function
      try {
        var xml = test.device.toXml();
        expect(xml).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should list device capabilities', function() {
      //test driver does not provide mechanisms to test this function
      try {
        var capabilities = test.device.getCapabilities();
        expect(capabilities).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should remove the node device from the host operating system', function() {
      //test driver does not provide mechanisms to test this function
      try {
        expect(test.device.destroy()).to.be.ok;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });
  });
});
