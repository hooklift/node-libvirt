'use strict';

var libvirt = require('../lib'),
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

    it('should create a transient pool', function(done) {
      var xml = fixture('storage_pool.xml');
      test.hypervisor.createStoragePool(xml, function(err, pool) {
        expect(err).to.not.exist;

        pool.getName(function(err, name) {
          expect(err).to.not.exist;
          expect(name).to.equal('virtimages');
          done();
        });
      });
    });

    it('should define a pool', function(done) {
      var xml = fixture('storage_pool.xml');
      test.hypervisor.defineStoragePool(xml, function(err, pool) {
        expect(err).to.not.exist;

        pool.getName(function(err, name) {
          expect(err).to.not.exist;
          expect(name).to.equal('virtimages');
          done();
        });
      });
    });

    it('should return volume names', function(done) {
      test.hypervisor.lookupStoragePoolByName('default-pool', function(err, pool) {
        expect(err).to.not.exist;
        expect(pool).to.exist;
        expect(pool._parent).to.exist;

        pool.getVolumes(function(err, volumes) {
          expect(err).to.not.exist;
          expect(volumes).to.be.instanceOf(Array);
          done();
        });
      });

    });
  });

  describe('methods', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;
        test.hypervisor.lookupStoragePoolByName('default-pool', function(err, pool) {
          expect(err).to.not.exist;
          expect(pool).to.exist;
          expect(pool._parent).to.exist;

          test.pool = pool;

          test.pool.isActive(function(err, active) {
            expect(err).to.not.exist;
            if (active) return done();

            test.pool.start(function(err, started) {
              expect(err).to.not.exist;
              done();
            });
          });
        });
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

    it('should indicate if autostart is enabled', function(done) {
      test.pool.getAutostart(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.false;
        done();
      });
    });

    it('should set autostart to start the pool at boot time', function(done) {
      test.pool.setAutostart(false, function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;

        test.pool.getAutostart(function(err, result) {
          expect(err).to.not.exist;

          // NOTE: seems broken on test-driver, should be: to.be.true
          expect(result).to.be.false;
          done();
        });
      });
    });

    it('should return its information', function(done) {
      // @todo: work on more idiomatic definitions of these
      //        constants: test.pool.VIR_STORAGE_POOL_RUNNING
      var storageRunning = 2;

      test.pool.getInfo(function(err, info) {
        expect(err).to.not.exist;
        expect(info).to.eql({
          state: storageRunning,
          capacity: 107374182400,
          allocation: 0,
          available: 107374182400
        });

        done();
      });
    });

    it('should be located by its name', function(done) {
      test.pool.getName(function(err, name) {
        expect(err).to.not.exist;
        expect(name).to.equal('default-pool');
        done();
      });
    });

    it('should be located by its uuid', function(done) {
      test.pool.getUUID(function(err, uuid) {
        expect(err).to.not.exist;

        test.hypervisor.lookupStoragePoolByUUID(uuid, function(err, pool) {
          expect(err).to.not.exist;
          expect(pool).to.exist;
          expect(pool._parent).to.exist;

          pool.getName(function(err, name) {
            expect(err).to.not.exist;
            expect(name).to.equal('default-pool');
            done();
          });
        });
      });
    });

    it('should return its name', function(done) {
      test.pool.getName(function(err, name) {
        expect(err).to.not.exist;
        expect(name).to.equal('default-pool');
        done();
      });
    });

    it('should return its uuid', function(done) {
      test.pool.getUUID(function(err, uuid) {
        expect(err).to.not.exist;

        test.hypervisor.lookupStoragePoolByUUID(uuid, function(err, pool) {
          expect(err).to.not.exist;
          expect(pool).to.exist;
          expect(pool._parent).to.exist;

          pool.getUUID(function(err, lookupUUID) {
            expect(err).to.not.exist;
            expect(lookupUUID).to.equal(uuid);
            done();
          });
        });
      });
    });

    it('should return its xml representation', function(done) {
      test.pool.toXml(function(err, xml) {
        expect(err).to.not.exist;
        expect(xml).to.match(/<name>default-pool<\/name>/);
        done();
      });
    });

    it('should show if the pool is active', function(done) {
      test.pool.isActive(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;
        done();
      });
    });

    // it('should start an already defined pool', function(done) {
    //   //workaround because test driver seems to start the pool when it's defined which is wrong
    //   //according to the documentation
    //   //see http://libvirt.org/html/libvirt-libvirt.html#virStoragePoolDefineXML

    //   function actualTest() {
    //     test.pool.start(function(err, startResult) {
    //       expect(err).to.not.exist;
    //       expect(startResult).to.be.true;

    //       test.pool.isActive(function(err, isActive) {
    //         expect(err).to.not.exist;
    //         expect(isActive).to.be.true;
    //         done();
    //       });
    //     });
    //   }

    //   test.pool.isActive(function(err, isActive) {
    //     expect(err).to.not.exist;
    //     if (isActive) {
    //       actualTest();
    //     } else {
    //       test.pool.stop(function(err, stopResult) {
    //         expect(err).to.not.exist;
    //         expect(stopResult).to.be.true;
    //         actualTest();
    //       });
    //     }
    //   });
    // });

    it('should show if the pool is persistent', function(done) {
      test.pool.isPersistent(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;
        done();
      });
    });

    it('should be refreshed', function(done) {
      test.pool.refresh(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.ok;
        done();
      });
    });

    it('should stop an started pool', function(done) {
      test.pool.stop(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.ok;
        done();
      });
    });

    it('should erase a pool', function(done) {
      test.pool.stop(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.ok;

        test.pool.erase([], function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.be.ok;
          done();
        });
      });
    });

    it('should be undefined', function(done) {
      test.pool.stop(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.ok;

        test.pool.undefine(function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.be.ok;

          test.hypervisor.lookupStoragePoolByName('default-pool', function(err, pool) {
            expect(err).to.exist;
            expect(pool).to.not.exist;
            done();
          });
        });
      });
    });
  });
});
