var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var sys = require('sys');
var libvirt = require('../build/Release/libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('test:///default');
var xml  = fixture('storage_volume.xml');
var pool = hypervisor.lookupStoragePoolByName('default-pool');
var volume;

module.exports = {
    'should be created': function(beforeExit, assert) {
        volume = pool.createVolume(xml);
        assert.eql(volume.getName(), 'sparse.img');
    },

    'should clone an existent volume': function(beforeExit, assert) {
        var xml = fixture('clone_volume.xml');
        var clone_vol = pool.cloneVolume(volume, xml);
        assert.eql(clone_vol.getName(), 'sparse_clone.img');
    },

    'should return volume information': function(beforeExit, assert) {
        var info = volume.getInfo();
        assert.eql(info.type, volume.VIR_STORAGE_VOL_FILE);
        //bytes
        assert.eql(info.capacity, 5368709120);
        assert.eql(info.allocation, 0);
    },

    'should be wiped': function(beforeExit, assert) {
        try {
            assert.ok(volume.wipe());
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return its key': function(beforeExit, assert) {
        assert.eql(volume.getKey(), '/default-pool/sparse.img');
    },

    'should return its name': function(beforeExit, assert) {
        assert.eql(volume.getName(), 'sparse.img');
    },

    'should return its path': function(beforeExit, assert) {
        assert.eql(volume.getPath(), '/default-pool/sparse.img');
    },

    'should return its xml description': function(beforeExit, assert) {
        assert.match(volume.toXml(), /<name>sparse.img<\/name>/);
    },

    'should be located by its key': function(beforeExit, assert) {
        var volume_  = hypervisor.lookupStorageVolumeByKey(volume.getKey());
        assert.eql(volume_.getName(), volume.getName());
    },

    'should be located by its name': function(beforeExit, assert) {
        var volume_ = pool.lookupVolumeByName(volume.getName());
        assert.eql(volume_.getKey(), volume.getKey());
    },

    'should be located by its path': function(beforeExit, assert) {
        var volume_ = hypervisor.lookupStorageVolumeByPath(volume.getPath());
        assert.eql(volume_.getKey(), volume.getKey());
    },

    'should be removed from the pool': function(beforeExit, assert) {
        assert.ok(volume.remove());
    }
};

