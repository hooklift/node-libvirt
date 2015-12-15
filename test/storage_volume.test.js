'use strict';

var libvirt = require('../lib'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

var test = {};
describe('Storage Volume', function() {
  before(function() {
    SegfaultHandler.registerHandler();
  });

  describe('storage pool methods', function() {
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

    it('should be created', function(done) {
      var xml = fixture('storage_volume.xml');
      test.pool.createVolume(xml, function(err, volume) {
        if (!!err) { console.log('\n\n\nERROR:\n'); console.log(err); console.log('\n\n\n'); }

        expect(err).to.not.exist;
        expect(volume._parent).to.exist;

        volume.getName(function(err, name) {
          expect(err).to.not.exist;
          expect(name).to.equal('sparse.img');

          volume.remove(function(err, result) {
            expect(err).to.not.exist;
            expect(result).to.be.true;
            done();
          });
        });
      });
    });

    it('should clone an existent volume', function(done) {
      var xml = fixture('storage_volume.xml');
      var clone_xml = fixture('clone_volume.xml');

      test.pool.createVolume(xml, function(err, volume) {
        expect(err).to.not.exist;

        volume.getName(function(err, name) {
          expect(err).to.not.exist;
          expect(name).to.equal('sparse.img');

          test.pool.cloneVolume(volume, clone_xml, function(err, cloneVolume) {
            expect(err).to.not.exist;
            expect(volume).to.exist;
            expect(volume._parent).to.exist;

            cloneVolume.getName(function(err, name) {
              expect(err).to.not.exist;
              expect(name).to.equal('sparse_clone.img');

              cloneVolume.remove(function(err, result) {
                expect(err).to.not.exist;
                expect(result).to.be.true;

                volume.remove(function(err, result) {
                  expect(err).to.not.exist;
                  expect(result).to.be.true;
                  done();
                });
              });
            });
          });
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

          var xml = fixture('storage_volume.xml');
          test.pool.createVolume(xml, function(err, volume) {
            expect(err).to.not.exist;
            expect(volume).to.exist;
            expect(volume._parent).to.exist;
            test.volume = volume;
            done();
          });
        });
      });
    });

    afterEach(function(done) {
      if (!!test.volume) {
        test.volume.remove(function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.be.true;
          test.volume = undefined;

          test.hypervisor.disconnect(function(err) {
            expect(err).to.not.exist;
            test.hypervisor = undefined;
            test.pool = undefined;
            done();
          });
        });
      } else {
        test.hypervisor.disconnect(function(err) {
          expect(err).to.not.exist;
          test.hypervisor = undefined;
          test.pool = undefined;
          done();
        });
      }
    });

    it('should return volume information', function(done) {
      test.volume.getInfo(function(err, info) {
        expect(err).to.not.exist;

        // @todo: find better way to store these constants
        // var storageVolumeFile = test.volume.VIR_STORAGE_VOL_FILE;
        var storageVolumeFile = 0;
        expect(info).to.eql({
          type: storageVolumeFile,
          capacity: 5368709120,
          allocation: 0
        });

        done();
      });
    });

    it('should be wiped', function(done) {
      test.volume.wipe(function(err, result) {
        // NOTE: not supported by test driver
        expect(err).to.exist;
        expect(result).to.not.exist;

        // expect(err).to.not.exist;
        // expect(result).to.be.true;
        done();
      });
    });

    it('should return its key', function(done) {
      test.volume.getKey(function(err, key) {
        expect(err).to.not.exist;
        expect(key).to.equal('/default-pool/sparse.img');
        done();
      });
    });

    it('should return its name', function(done) {
      test.volume.getName(function(err, name) {
        expect(err).to.not.exist;
        expect(name).to.equal('sparse.img');
        done();
      });
    });

    it('should return its path', function(done) {
      test.volume.getPath(function(err, path) {
        expect(err).to.not.exist;
        expect(path).to.equal('/default-pool/sparse.img');
        done();
      });
    });

    it('should return its xml description', function(done) {
      test.volume.toXml(function(err, xml) {
        expect(err).to.not.exist;
        expect(xml).to.match(/<name>sparse.img<\/name>/);
        done();
      });
    });

    it('should be located by its key', function(done) {
      test.volume.getKey(function(err, key) {
        expect(err).to.not.exist;
        test.hypervisor.lookupStorageVolumeByKey(key, function(err, volume) {
          expect(err).to.not.exist;
          expect(volume).to.exist;
          expect(volume._parent).to.exist;

          volume.getName(function(err, lookupName) {
            expect(err).to.not.exist;

            test.volume.getName(function(err, name) {
              expect(err).to.not.exist;
              expect(lookupName).to.equal(name);
              done();
            });
          });
        });
      });
    });

    it('should be located by its name', function(done) {
      test.volume.getName(function(err, name) {
        expect(err).to.not.exist;
        test.pool.lookupStorageVolumeByName(name, function(err, volume) {
          expect(err).to.not.exist;
          expect(volume).to.exist;
          expect(volume._parent).to.exist;

          volume.getKey(function(err, lookupKey) {
            expect(err).to.not.exist;

            test.volume.getKey(function(err, key) {
              expect(err).to.not.exist;
              expect(lookupKey).to.equal(key);
              done();
            });
          });
        });
      });
    });

    it('should be located by its path', function(done) {
      test.volume.getPath(function(err, path) {
        expect(err).to.not.exist;
        test.hypervisor.lookupStorageVolumeByPath(path, function(err, volume) {
          expect(err).to.not.exist;
          expect(volume).to.exist;
          expect(volume._parent).to.exist;

          volume.getKey(function(err, lookupKey) {
            expect(err).to.not.exist;

            test.volume.getKey(function(err, key) {
              expect(err).to.not.exist;
              expect(lookupKey).to.equal(key);
              done();
            });
          });
        });
      });
    });

    it('should be removed from the pool', function(done) {
      test.volume.remove(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;
        test.volume = undefined;
        done();
      });
    });
  });
});
