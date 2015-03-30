'use strict';

var libvirt = require('../build/Release/libvirt'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

var test = {};
describe('Storage Pool', function() {
  before(function() {
    SegfaultHandler.registerHandler();
  });

  describe('hypervisor methods', function() {
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
        test.hypervisor = undefined;
        done();
      });
    });

    it('should create a transient pool', function() {
      var xml = fixture('storage_pool.xml');
      var pool_ = test.hypervisor.createStoragePool(xml);
      expect(pool_.getName()).to.equal('virtimages');
    });

    it('should define a pool', function() {
      var xml = fixture('storage_pool.xml');
      var pool_ = test.hypervisor.defineStoragePool(xml);
      expect(pool_.getName()).to.equal('virtimages');
    });

    // it('should start an already defined pool', function() {
    //   //workaround because test driver seems to start the pool when it's defined which is wrong
    //   //according to the documentation
    //   //see http://libvirt.org/html/libvirt-libvirt.html#virStoragePoolDefineXML
    //   if (pool.isActive()) {
    //     pool.stop();
    //   }
    //   assert.ok(pool.start());
    //   assert.ok(pool.isActive());
    // });

    it('should return volumes names', function() {
      var pool_ = test.hypervisor.lookupStoragePoolByName('default-pool');
      var volumes = pool_.getVolumes();
      expect(volumes).to.be.instanceOf(Array);
    });
  });

  describe('methods', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;
        test.pool = test.hypervisor.lookupStoragePoolByName('default-pool');
        done();
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        test.hypervisor = undefined;
        test.pool = undefined;
        done();
      });
    });

    it('should indicate if autostart is enabled', function() {
      expect(test.pool.getAutostart).to.be.ok;
    });

    it('should set autostart to start the pool at boot time', function() {
      test.pool.setAutostart(false);
      expect(test.pool.getAutostart()).to.be.true;

      // NOTE: doesn't work with test driver
      // expect(test.pool.getAutostart()).to.be.false;
    });

    it('should return its information', function() {
      var info = test.pool.getInfo();
      expect(info).to.eql({
        state: test.pool.VIR_STORAGE_POOL_RUNNING,
        capacity: 107374182400,
        allocation: 0,
        available: 107374182400
      });
    });

    it('should be located by its name', function() {
      expect(test.pool.getName()).to.equal('default-pool');
    });

    it('should be located by its uuid', function() {
      var uuid = test.pool.getUUID();
      var pool_ = test.hypervisor.lookupStoragePoolByUUID(uuid);
      expect(pool_.getName()).to.equal('default-pool');
    });

    it('should return its name', function() {
      expect(test.pool.getName()).to.equal('default-pool');
    });

    it('should return its uuid', function() {
      var uuid = test.pool.getUUID();
      var pool_ = test.hypervisor.lookupStoragePoolByUUID(uuid);
      expect(pool_.getUUID()).to.equal(uuid);
    });

    it('should return its xml representation', function() {
      expect(test.pool.toXml([])).to.match(/<name>default-pool<\/name>/);
    });

    it('should show if the pool is active', function() {
      expect(test.pool.isActive()).to.be.true;
    });

    it('should show if the pool is persistent', function() {
      expect(test.pool.isPersistent()).to.be.true;
    });

    it('should be refreshed', function() {
      expect(test.pool.refresh()).to.be.ok;
    });

    it('should stop an started pool', function() {
      expect(test.pool.stop()).to.be.ok;
    });

    it('should erase a pool', function() {
      expect(test.pool.erase()).to.be.ok;
    });

    it('should be undefined', function() {
      expect(test.pool.undefine()).to.be.ok;
      expect(function() {
        test.hypervisor.lookupStoragePoolByName('default-pool');
      }).to.throw([Error]);

      // NOTE: the above is strange and broken!

      // try { assert.ok(pool.undefine()); } catch (err) {}
      // try {
      //   hypervisor.lookupStoragePoolByName('default-pool');
      // } catch (error) {
      //   assert.eql(error.message, 'Storage pool not found');
      // }
    });
  });
});
