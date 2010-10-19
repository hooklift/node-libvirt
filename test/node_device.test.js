require.paths.unshift('build/default/src');
var sys = require('sys');
var libvirt = require('libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;
var hypervisor = new Hypervisor('test:///default');

//TODO create a Node class and add detach attach
module.exports = {
    'should create a node device using its xml description': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            var xml = fixture('node_device.xml');
            var device = hypervisor.createNodeDevice(xml);
            assert.isDefined(device);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_INTERNAL_ERROR);
        }
    },

    'should lookup a node device by name': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            var devices = hypervisor.getNodeDevicesNames();
            var device = hypervisor.lookupNodeDeviceByName(devices[0]);
            assert.isDefined(device);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should detach the device from the node itself in order to be assigned to a guest domain': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            assert.ok(device.detach());
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should reattach a previously detached node device': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            assert.ok(device.reattach());
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should reset the node device a previously detached node device': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            assert.ok(device.reset());
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return the device name': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            var name = device.getName();
            assert.isDefined(name);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return the device parent name': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            var parent = device.getParentName();
            assert.isDefined(parent);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return de device xml description': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            var xml = device.toXml();
            assert.ok(xml);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should list device capabilities': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            var capabilities = device.getCapabilities();
            assert.ok(capabilities);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should remove the node device from the host operating system': function(assert) {
        //test driver does not provide mechanisms to test this function
        try {
            assert.ok(device.destroy());
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    }
};

