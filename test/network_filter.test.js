var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var sys = require('sys');
var libvirt = require('../build/Release/libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('test:///default');

module.exports = {
    'should define network filter from its xml description': function(beforeExit, assert) {
        //test driver does not provide mechanisms to test this function
        try {
            var xml = fixture('network_filter.xml');
            assert.ok(hypervisor.defineNetworkFilter(xml));
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return the network filter name': function(beforeExit, assert) {
        //test driver does not provide mechanisms to test this function
        //filter.getName().should_be 'default'
    },

    'should return the network filter UUID': function(beforeExit, assert) {
        //test driver does not provide mechanisms to test this function
        //filter.GetUUID().should_not_be undefined
    },

    'should return the xml description of the network filter': function(beforeExit, assert) {
        //test driver does not provide mechanisms to test this function
        //var xml = filter.toXml();
    },

    'should look up the network filter based in its name': function(beforeExit, assert) {
        //test driver does not provide mechanisms to test this function
        try {
            var filter = hypervisor.lookupNetworkFilterByName('test-eth0');
            assert.eql(filter.getName(), 'default');
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should look up the network filter based in its UUID': function(beforeExit, assert) {
        //test driver does not provide mechanisms to test this function
        try {
            var filter = hypervisor.lookupNetworkFilterByName('test-eth0');
            var filter2 = hypervisor.lookupNetworkFilterByUUID(filter.getUUID());
            assert.eql(filter2.getName(), 'test-eth0');
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should undefine the network filter': function(beforeExit, assert) {
        //test driver does not provide mechanisms to test this function
        //filter.undefine().should_be true
    }
};

