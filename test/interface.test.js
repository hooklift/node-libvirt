require.paths.unshift('build/default/src');
var sys = require('sys');
var libvirt = require('libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('test:///default');
var interface  = hypervisor.lookupInterfaceByName('eth1');

module.exports = {
    'should define the interface from its xml description': function(assert) {
        var xml = fixture('interface.xml');
        var iface = hypervisor.defineInterface(xml);
        assert.eql(iface.getName(), 'eth0');
    },

    'should undefine the interface': function(assert) {
        var iface = hypervisor.lookupInterfaceByName('eth0');
        assert.eql(iface.undefine(), true);
    },

    'should be located through its name': function(assert) {
        var iface = hypervisor.lookupInterfaceByName('eth1');
        assert.eql(iface.getName(), 'eth1');
    },

    'should be located through its mac address': function(assert) {
        var iface = hypervisor.lookupInterfaceByMacAddress('aa:bb:cc:dd:ee:ff');
        assert.eql(iface.getName(), 'eth1');
    },

    'should stop': function(assert) {
        assert.eql(interface.stop(), true);
    },

    'should start': function(assert) {
        assert.eql(interface.start(), true);
    },

    'should indicate if is active and running': function(assert) {
        assert.eql(interface.isActive(), true);
    },

    'should return the name': function(assert) {
        assert.eql(interface.getName(), 'eth1');
    },

    'should return the mac address': function(assert) {
        assert.eql(interface.getMacAddress(), 'aa:bb:cc:dd:ee:ff');
    },

    'should return its xml description': function(assert) {
        assert.match(interface.toXml([]), /eth1/);
    }
};

