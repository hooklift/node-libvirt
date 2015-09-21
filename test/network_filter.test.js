'use strict';

var libvirt = require('../lib'),
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

  it('should define network filter from its xml description', function(done) {
    // NOTE: test driver does not provide mechanisms to test this function
    var xml = fixture('network_filter.xml');
    test.hypervisor.defineNetworkFilter(xml, function(err, filter) {
      expect(err.code).to.equal(libvirt.VIR_ERR_NO_SUPPORT);
      // expect(err).to.not.exist;
      // expect(filter).to.exist;
      done();
    });
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

  it('should look up the network filter based in its name', function(done) {
    // NOTE: test driver does not provide mechanisms to test this function
    test.hypervisor.lookupNetworkFilterByName('test-eth0', function(err, filter) {
      expect(err.code).to.equal(libvirt.VIR_ERR_NO_SUPPORT);
      done();

      // expect(err).to.not.exist;

      // filter.getName(function(err, name) {
      //   expect(err).to.not.exist;
      //   expect(name).to.equal('default');
      //   done();
      // });
    });
  });

  it('should look up the network filter based in its UUID', function(done) {
    // NOTE: test driver does not provide mechanisms to test this function
    test.hypervisor.lookupNetworkFilterByName('test-eth0', function(err, filter1) {
      expect(err.code).to.equal(libvirt.VIR_ERR_NO_SUPPORT);
      done();

      // expect(err).to.not.exist;

      // filter.getUUID(function(err, uuid) {
      //   expect(err).to.not.exist;

      //   test.hypervisor.lookupNetworkFilterByUUID(function(err, filter2) {
      //     expect(err).to.not.exist;

      //     filter2.getName(function(err, name) {
      //       expect(err).to.not.exist();
      //       expect(name).to.equal('test-eth0');
      //       done();
      //     });
      //   });
      // });
    });
  });

  it('should undefine the network filter', function() {
    //test driver does not provide mechanisms to test this function
    //filter.undefine().should_be true
  });
});
