var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var sys = require('sys');
var libvirt = require('../build/Release/libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('test:///default');
var pool = hypervisor.lookupStoragePoolByName('default-pool');
var xml = fixture('storage_pool.xml');

module.exports = {
    'should create a transient pool': function(beforeExit, assert) {
        var pool_ = hypervisor.createStoragePool(xml);
        assert.eql(pool_.getName(), 'virtimages');
    },

    'should define a pool': function(beforeExit, assert) {
        var pool_ = hypervisor.defineStoragePool(xml);
        assert.eql(pool_.getName(), 'virtimages');
    },

    'should start an already defined pool': function(beforeExit, assert) {
        //workaround because test driver seems to start the pool when it's defined which is wrong
        //according to the documentation
        //see http://libvirt.org/html/libvirt-libvirt.html#virStoragePoolDefineXML
        if(pool.isActive()) {
            pool.stop();
        }
        assert.ok(pool.start());
        assert.ok(pool.isActive());
    },

    'should return volumes names': function(beforeExit, assert) {
        var pool_ = hypervisor.lookupStoragePoolByName('default-pool');
        var volumes = pool_.getVolumes();
        assert.ok(volumes instanceof Array);
    },

    'should indicate if autostart is enabled': function(beforeExit, assert) {
        assert.ok(pool.getAutostart());
    },

    'should set autostart to start the pool at boot time': function(beforeExit, assert) {
        pool.setAutostart(false);
        assert.eql(pool.getAutostart(), false);
    },

    'should return its information': function(beforeExit, assert) {
        var info = pool.getInfo();
        assert.eql(info.state, pool.VIR_STORAGE_POOL_RUNNING);
        assert.eql(info.capacity, 107374182400);
        assert.eql(info.allocation, 0);
        assert.eql(info.available, 107374182400);
    },

    'should be located by its name': function(beforeExit, assert) {
        assert.eql(pool.getName(), 'default-pool');
    },

    'should be located by its uuid': function(beforeExit, assert) {
        var uuid = pool.getUUID();
        var pool_ = hypervisor.lookupStoragePoolByUUID(uuid);
        assert.eql(pool_.getName(), 'default-pool');
    },

    'should return its name': function(beforeExit, assert) {
        assert.eql(pool.getName(), 'default-pool');
    },

    'should return its uuid': function(beforeExit, assert) {
        var uuid = pool.getUUID();
        pool_ = hypervisor.lookupStoragePoolByUUID(uuid);
        assert.eql(pool_.getUUID(), uuid);
    },

    'should return its xml representation': function(beforeExit, assert) {
        assert.match(pool.toXml([]), /<name>default-pool<\/name>/);
    },

    'should show if the pool is active': function(beforeExit, assert) {
        assert.ok(pool.isActive());
    },

    'should show if the pool is persistent': function(beforeExit, assert) {
        assert.ok(pool.isPersistent());
    },

    'should be refreshed': function(beforeExit, assert) {
        assert.ok(pool.refresh());
    },

    'should stop an started pool': function(beforeExit, assert) {
        assert.ok(pool.stop());
    },

    'should erase a pool': function(beforeExit, assert) {
        assert.ok(pool.erase());
    },

    'should be undefined': function(beforeExit, assert) {
        try { assert.ok(pool.undefine()); } catch(err) {}
        try {
            hypervisor.lookupStoragePoolByName('default-pool');
        } catch(error) {
            assert.eql(error.message, 'Storage pool not found');
        }
    }
};

