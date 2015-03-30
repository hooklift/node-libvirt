'use strict';

var libvirt = require('../build/Release/libvirt'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

var test = {};
describe('Storage Volume', function() {
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

  describe('storage pool methods', function() {
    beforeEach(function() {
      test.pool = test.hypervisor.lookupStoragePoolByName('default-pool');
    });

    afterEach(function() {
      test.pool = undefined;
    });

    it('should be created', function() {
      var xml = fixture('storage_volume.xml');
      var volume = test.pool.createVolume(xml);
      expect(volume.getName()).to.equal('sparse.img');
      expect(volume.remove()).to.be.ok;
    });

    it('should clone an existent volume', function() {
      var xml = fixture('storage_volume.xml');
      var volume = test.pool.createVolume(xml);
      expect(volume.getName()).to.equal('sparse.img');

      var clone_xml = fixture('clone_volume.xml');
      var clone_vol = test.pool.cloneVolume(volume, xml);
      expect(clone_vol.getName()).to.equal('sparse_clone.img');
      expect(clone_vol.remove()).to.be.ok;
      expect(volume.remove()).to.be.ok;
    });
  });

  describe('methods', function() {
    beforeEach(function() {
      try {
        test.pool = test.hypervisor.lookupStoragePoolByName('default-pool');

        var xml = fixture('storage_volume.xml');
        test.volume = test.pool.createVolume(xml);
      } catch (err) {
        console.log(err);
      }
    });

    afterEach(function() {
      test.pool = undefined;
      test.volume = undefined;
    });

    it('should return volume information', function() {
      var info = test.volume.getInfo();
      expect(info.type).to.equal(test.volume.VIR_STORAGE_VOL_FILE);
      expect(info.capacity).to.equal(5368709120);
      expect(info.allocation).to.equal(0);
    });

    it('should be wiped', function() {
      expect(function() { test.volume.wipe(); }).to.throw(Error);

      // try {
      //   assert.ok(volume.wipe());
      // } catch (error) {
      //   assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
      // }
    });

    it('should return its key', function() {
      expect(test.volume.getKey()).to.equal('/default-pool/sparse.img');
    });

    it('should return its name', function() {
      expect(test.volume.getName()).to.equal('sparse.img');
    });

    it('should return its path', function() {
      expect(test.volume.getPath()).to.equal('/default-pool/sparse.img');
    });

    it('should return its xml description', function() {
      expect(test.volume.toXml()).to.match(/<name>sparse.img<\/name>/);
    });

    it('should be located by its key', function() {
      var volume_ = test.hypervisor.lookupStorageVolumeByKey(test.volume.getKey());
      expect(volume_.getName()).to.equal(test.volume.getName());
    });

    it('should be located by its name', function() {
      var volume_ = test.pool.lookupVolumeByName(test.volume.getName());
      expect(volume_.getKey()).to.equal(test.volume.getKey());
    });

    it('should be located by its path', function() {
      var volume_ = test.hypervisor.lookupStorageVolumeByPath(test.volume.getPath());
      expect(volume_.getKey()).to.equal(test.volume.getKey());
    });

    it('should be removed from the pool', function() {
      expect(test.volume.remove()).to.be.ok;
    });
  });
});
