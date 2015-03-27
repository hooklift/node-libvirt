var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var sys = require('sys');
var libvirt = require('../build/Release/libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('test:///default');
var network = hypervisor.lookupNetworkByName('default');

module.exports = {
  'should create and start an already defined network': function(beforeExit, assert) {
    var xml = fixture('network.xml');
    var net = hypervisor.defineNetwork(xml);
    assert.eql(net.start(), true);
  },

  'should create and start a new virtual network from its xml description': function(beforeExit, assert) {
    var xml = fixture('network.xml');
    var net = hypervisor.createNetwork(xml);
    assert.eql(net.getName(), 'test');
  },

  'should lookup the network by name': function(beforeExit, assert) {
    var net = hypervisor.lookupNetworkByName('default');
    assert.eql(net.getName(), 'default');
  },

  'should lookup the network by uuid': function(beforeExit, assert) {
    var uuid = network.getUUID();
    var net = hypervisor.lookupNetworkByUUID(uuid);
    assert.eql(net.getName(), 'default');
  },

  'should determinate if the network has a persistent configuration': function(beforeExit, assert) {
    assert.eql(network.isPersistent(), true);
  },

  'should determinate if the network is currently running': function(beforeExit, assert) {
    assert.ok(network.isActive());
  },

  'should provide a xml network description': function(beforeExit, assert) {
    var xml = network.toXml([]);
    assert.match(xml, /<name>default<\/name>/);
  },

  'should return the network uuid': function(beforeExit, assert) {
    assert.ok(network.getUUID());
  },

  'should return the network name': function(beforeExit, assert) {
    assert.eql(network.getName(), 'default');
  },

  'should return the bridge interface name': function(beforeExit, assert) {
    var name = network.getBridgeName();
    assert.eql(name, 'virbr0');
  },

  'should indicate if the network is configured to be automatically started when the host boots': function(beforeExit, assert) {
    assert.eql(network.getAutostart(), true);
  },

  'should configure the network to be automatically started when the host boots': function(beforeExit, assert) {
    assert.ok(network.setAutostart(false));
    assert.eql(network.getAutostart(), false);
    assert.ok(network.setAutostart(true));
    assert.eql(network.getAutostart(), true);
  },

  'should define a network from its xml description': function(beforeExit, assert) {
    var xml = fixture('network.xml');
    var net = hypervisor.defineNetwork(xml);
    assert.eql(net.getName(), 'test');
  },

  'should undefine a network': function(beforeExit, assert) {
    var net = hypervisor.lookupNetworkByName('test');
    assert.ok(net.destroy());
    try { assert.ok(net.undefine()); } catch (err) {}
  },

  'should destroy a network': function(beforeExit, assert) {
    assert.ok(network.destroy());
  }
};

