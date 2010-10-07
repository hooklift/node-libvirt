require.paths.unshift('build/default/src');
var sys = require('sys');
var libvirt = require('node-libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('test:///default');
var network = hypervisor.lookupNetworkByName('default');

module.exports = {
    'should create and start an already defined network': function(assert) {
        var xml = fixture('network.xml');
        var net = hypervisor.defineNetwork(xml);
        assert.eql(net.start(), true);
    },

    'should create and start a new virtual network from its xml description': function(assert) {
        var xml = fixture('network.xml');
        var net = hypervisor.createNetwork(xml);
        assert.eql(net.getName(), 'test');
    },

    'should lookup the network by name': function(assert) {
        var net = hypervisor.lookupNetworkByName('default');
        assert.eql(net.getName(), 'default');
    },

    'should lookup the network by uuid': function(assert) {
        var uuid = network.getUUID();
        var net = hypervisor.lookupNetworkByUUID(uuid);
        assert.eql(net.getName(), 'default');
    },

    'should determinate if the network has a persistent configuration': function(assert) {
        assert.eql(network.isPersistent(), true);
    },

    'should determinate if the network is currently running': function(assert) {
        assert.ok(network.isActive());
    },

    'should provide a xml network description': function(assert) {
        var xml = network.toXml([]);
        assert.match(xml, /<name>default<\/name>/);
    },

    'should return the network uuid': function(assert) {
        assert.ok(network.getUUID());
    },

    'should return the network name': function(assert) {
        assert.eql(network.getName(), 'default');
    },

    'should return the bridge interface name': function(assert) {
        var name = network.getBridgeName();
        assert.eql(name, 'virbr0');
    },

    'should indicate if the network is configured to be automatically started when the host boots': function(assert) {
        assert.eql(network.getAutostart(), true);
    },

    'should configure the network to be automatically started when the host boots': function(assert) {
        assert.ok(network.setAutostart(false));
        assert.eql(network.getAutostart(), false);
        assert.ok(network.setAutostart(true));
        assert.eql(network.getAutostart(), true);
    },

    'should define a network from its xml description': function(assert) {
        var xml = fixture('network.xml');
        var net = hypervisor.defineNetwork(xml);
        assert.eql(net.getName(), 'test');
    },

    'should undefine a network': function(assert) {
        var net = hypervisor.lookupNetworkByName('test');
        assert.ok(net.destroy());
        assert.ok(net.undefine());
    },

    'should destroy a network': function(assert) {
        assert.ok(network.destroy());
    }
};

