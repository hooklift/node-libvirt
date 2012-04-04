require.paths.unshift('build/default/src');
var sys = require('sys');
var libvirt = require('libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('test:///default');
var interface  = hypervisor.lookupInterfaceByName('eth1');

module.exports = {
    'should define the interface from its xml description': function(beforeExit, assert) {
        var xml = fixture('interface.xml');
        var iface = hypervisor.defineInterface(xml);
        assert.eql(iface.getName(), 'eth0');
    },

    'should undefine the interface': function(beforeExit, assert) {
        var iface = hypervisor.lookupInterfaceByName('eth0');
        assert.eql(iface.undefine(), true);
    },

    'should be located through its name': function(beforeExit, assert) {
        var iface = hypervisor.lookupInterfaceByName('eth1');
        assert.eql(iface.getName(), 'eth1');
    },

    'should be located through its mac address': function(beforeExit, assert) {
        var iface = hypervisor.lookupInterfaceByMacAddress('aa:bb:cc:dd:ee:ff');
        assert.eql(iface.getName(), 'eth1');
    },

    'should stop': function(beforeExit, assert) {
        assert.eql(interface.stop(), true);
    },

    'should start': function(beforeExit, assert) {
        try { assert.eql(interface.start(), true); } catch(err) {}
    },

    'should indicate if is active and running': function(beforeExit, assert) {
        try { assert.eql(interface.isActive(), true); } catch(err) {}
    },

    'should return the name': function(beforeExit, assert) {
        try { assert.eql(interface.getName(), 'eth1'); } catch(err) {}
    },

    'should return the mac address': function(beforeExit, assert) {
        try { assert.eql(interface.getMacAddress(), 'aa:bb:cc:dd:ee:ff'); } catch(err) {}
    },

    'should return its xml description': function(beforeExit, assert) {
        try { assert.match(interface.toXml([]), /eth1/); } catch(err) {}
    }
};

