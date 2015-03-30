'use strict';

var libvirt = require('../build/Release/libvirt'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

var test = {};
describe('Network Filter', function() {
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

  it('should define network filter from its xml description', function() {
    //test driver does not provide mechanisms to test this function
    try {
      var xml = fixture('network_filter.xml');
      expect(test.hypervisor.defineNetworkFilter(xml)).to.be.ok;
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should return the network filter name', function() {
    //test driver does not provide mechanisms to test this function
    //filter.getName().should_be 'default'
  });

  it('should return the network filter UUID', function() {
    //test driver does not provide mechanisms to test this function
    //filter.GetUUID().should_not_be undefined
  });

  it('should return the xml description of the network filter', function() {
    //test driver does not provide mechanisms to test this function
    //var xml = filter.toXml();
  });

  it('should look up the network filter based in its name', function() {
    //test driver does not provide mechanisms to test this function
    try {
      var filter = test.hypervisor.lookupNetworkFilterByName('test-eth0');
      expect(filter.getName()).to.equal('default');
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should look up the network filter based in its UUID', function() {
    //test driver does not provide mechanisms to test this function
    try {
      var filter = test.hypervisor.lookupNetworkFilterByName('test-eth0');
      var filter2 = test.hypervisor.lookupNetworkFilterByUUID(filter.getUUID());
      expect(filter2.getName()).to.equal('test-eth0');
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should undefine the network filter', function() {
    //test driver does not provide mechanisms to test this function
    //filter.undefine().should_be true
  });
});
