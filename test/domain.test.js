'use strict';

var libvirt = require('../lib'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    h = require('./lib/helper'),
    semver = require('semver'),
    expect = require('chai').expect;

var test = {};
describe('Domain', function() {
  before(function() {
    SegfaultHandler.registerHandler();
    return h.getLibVirtVersion()
      .then(function(version) {
        test.version = version;
      });
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
        done();
      });
    });

    it('should lookup a domain by id', function(done) {
      test.hypervisor.lookupDomainById(1, function(err, domain) {
        expect(err).to.not.exist;
        expect(domain).to.exist;
        expect(domain._parent).to.exist;
        done();
      });
    });

    it('should create a persistent Domain from its XML Description', function(done) {
      var xml = h.fixture('domain.xml');
      test.hypervisor.createDomain(xml, function(err, domain) {
        expect(err).to.not.exist;
        expect(domain).to.exist;
        expect(domain._parent).to.exist;

        test.hypervisor.lookupDomainByName('nodejs-test', function(err, lookupDomain) {
          expect(err).to.not.exist;
          expect(lookupDomain).to.exist;
          expect(lookupDomain._parent).to.exist;

          lookupDomain.getName(function(err, name) {
            expect(err).to.not.exist;
            expect(name).to.equal('nodejs-test');

            lookupDomain.destroy(function(err, result) {
              expect(err).to.not.exist;
              expect(result).to.be.true;
              done();
            });
          });
        });
      });
    });

    it('should {,un}define a persistent Domain', function(done) {
      var xml = h.fixture('domain.xml');
      test.hypervisor.defineDomain(xml, function(err, domain) {
        expect(err).to.not.exist;
        expect(domain).to.exist;
        expect(domain._parent).to.exist;

        test.hypervisor.lookupDomainByName('nodejs-test', function(err, lookupDomain) {
          expect(err).to.not.exist;
          expect(lookupDomain).to.exist;
          expect(lookupDomain._parent).to.exist;

          lookupDomain.undefine(function(err, result) {
            expect(err).to.not.exist;
            expect(result).to.be.true;

            test.hypervisor.lookupDomainByName('nodejs-test', function(err, lookupDomain) {
              expect(err).to.exist;
              done();
            });
          });
        });
      });
    });
  });

  describe('actions', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;

        test.hypervisor.lookupDomainById(1, function(err, domain) {
          expect(err).to.not.exist;
          expect(domain).to.exist;
          expect(domain._parent).to.exist;
          test.domain = domain;
          done();
        });
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        done();
      });
    });

    it('should reset the domain', function(done) {
      test.domain.reset(function(err, result) {
        expect(err).to.exist;

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // expect(result).to.be.true;
        done();
      });
    });

    it('should reboot the domain', function(done) {
      test.domain.reboot(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;
        done();
      });
    });

    it('should save and restore the domain', function(done) {
      test.domain.getName(function(err, name) {
        expect(err).to.not.exist;

        var path = '/tmp/' + name + '-saved.img';
        test.domain.save(path, function(err, saved) {
          expect(err).to.not.exist;
          expect(saved).to.be.true;

          test.hypervisor.restoreDomain(path, function(err, restored) {
            expect(err).to.not.exist;
            expect(saved).to.be.true;
            done();
          });
        });
      });
    });

    it('should suspend and resume the domain', function(done) {
      test.domain.suspend(function(err, suspended) {
        expect(err).to.not.exist;
        expect(suspended).to.be.true;

        test.domain.resume(function(err, resumed) {
          expect(err).to.not.exist;
          expect(resumed).to.be.true;
          done();
        });
      });
    });

    it('should shutdown the domain', function(done) {
      test.domain.shutdown(function(err, shutdown) {
        expect(err).to.not.exist;
        expect(shutdown).to.be.true;

        // reset state
        test.domain.start(function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.be.true;
          done();
        });
      });
    });

    it('should dump the core of a domain on a given file for analysis', function(done) {
      var path = '/tmp/dumpcore-test.txt';
      test.domain.coreDump(path, function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;
        done();
      });
    });

    it('should save a managed image of the domain', function(done) {
      if (semver.lt(test.version, '1.0.0')) { return done(); }

      // NOTE: test driver doesn't support these functions in 0.9.x
      test.domain.managedSave(function(err, saved) {
        expect(err).to.not.exist;
        expect(saved).to.be.true;

        test.domain.hasManagedSaveImage(function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.be.true;
          done();
        });
      });
    });

    it('should remove a managed image of the domain', function(done) {
      if (semver.lt(test.version, '1.0.0')) { return done(); }

      test.domain.managedSave(function(err, saved) {
        expect(err).to.not.exist;
        expect(saved).to.be.true;

        test.domain.managedSaveRemove(function(err, removed) {
          expect(err).to.not.exist;
          expect(removed).to.be.true;

          test.domain.hasManagedSaveImage(function(err, result) {
            expect(err).to.not.exist;
            expect(result).to.be.false;
            done();
          });
        });
      });
    });

    it('should request that the current background job be aborted at the soonest opportunity', function(done) {
      test.domain.abortCurrentJob(function(err, result) {
        expect(err).to.exist;

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // expect(result).to.be.true;
        done();
      });
    });

    it('should attach a device', function(done) {
      var xml = h.fixture('device.xml');
      test.domain.attachDevice(xml, function(err, result) {
        expect(err).to.exist;

        // NOTE: not supported by current driver
        // expect(err).to.not.exist;
        // expect(result).to.be.true;

        done();
      });
    });

    it('should detach a device', function(done) {
      var xml = h.fixture('device.xml');
      test.domain.detachDevice(xml, function(err, result) {
        expect(err).to.exist;

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // expect(result).to.be.true;

        done();
      });
    });

    it('should update a device', function(done) {
      var xml = h.fixture('device_update.xml');
      var flags = [libvirt.VIR_DOMAIN_DEVICE_MODIFY_CONFIG];
      test.domain.updateDevice(xml, flags, function(err, result) {
        expect(err).to.exist;

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // expect(result).to.be.true;

        done();
      });
    });

    // it('should migrate a domain to another hypervisor through a hypervisor connection', function(done) {
    //   var hypervisor2 = new libvirt.Hypervisor('test:///default');
    //   var flags = [
    //     test.domain.VIR_MIGRATE_LIVE,
    //     test.domain.VIR_MIGRATE_PEER2PEER,
    //     test.domain.VIR_MIGRATE_PAUSED,
    //     test.domain.VIR_MIGRATE_PERSIST_DEST
    //   ];

    //   hypervisor2.connect(function(err) {
    //     expect(err).to.not.exist;
    //     test.domain.migrate({ dest_hypervisor: hypervisor2, dest_name: 'test2', dest_uri: '', bandwidth: 100, flags: flags }, function(err, domain) {
    //       expect(err).to.exist;
    //       expect(err.code).to.be.equal(err.VIR_ERR_NO_SUPPORT);

    //       // NOTE: not supported by test driver
    //       // expect(err).to.not.exist;
    //       // expect(domain).to.exist;

    //       done();
    //     });
    //   });
    // });

    it('should migrate a domain to another hypervisor through an uri', function(done) {
      var flags = [
        test.domain.VIR_MIGRATE_LIVE,
        test.domain.VIR_MIGRATE_PEER2PEER,
        test.domain.VIR_MIGRATE_PAUSED,
        test.domain.VIR_MIGRATE_PERSIST_DEST
      ];

      test.domain.migrate({ dest_uri: 'test:///default', dest_name: 'test2', bandwidth: 100, flags: flags }, function(err) {
        expect(err).to.exist;
        // some libvirt versions report different error codes.
        var possibleErrors = [
          libvirt.VIR_ERR_OPERATION_INVALID,
          libvirt.VIR_ERR_NO_SUPPORT,
          libvirt.VIR_ERR_ARGUMENT_UNSUPPORTED
        ];

        expect(possibleErrors).to.include(err.code);
        // NOTE: not supported by test driver
        // expect(err).to.not.exist;

        done();
      });
    });

    it('should allow to change real CPUs, which can be allocated to a virtual CPU', function(done) {
      test.domain.getVcpus(function(err, res) {
        expect(err).to.not.exist;

        var affinity = res[0].affinity;
        affinity[0].usable = false;
        affinity[1].usable = false;

        test.domain.pinVcpu(res[0].number, affinity, function(err, result) {
          expect(err).to.not.exist;
          expect(result).to.be.true;
          done();
        });
      });
    });

    it('should allow to read the domain\'s memory content and return it in a Buffer object', function(done) {
      var physical = [test.domain.VIR_MEMORY_PHYSICAL];
      var virtual = [test.domain.VIR_MEMORY_VIRTUAL];

      // support different errors for different libvirt versions
      var possibleErrors = [ libvirt.VIR_ERR_INVALID_ARG, libvirt.VIR_ERR_NO_SUPPORT ];

      test.domain.memoryPeek(0, 1024, physical, function(err, res) {
        expect(err).to.exist;
        expect(possibleErrors).to.include(err.code);

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // expect(res).to.be.instanceof(Buffer);

        test.domain.memoryPeek(0, 1024, virtual, function(err, res) {
          expect(err).to.exist;
          expect(possibleErrors).to.include(err.code);

          // NOTE: not supported by test driver
          //expect(err).to.not.exist;
          //expect(res).to.be.instanceof(Buffer);

          done();
        });
      });
    });

    it('should allow to read the content of a domain\'s disk device and return it in a Buffer object', function(done) {
      test.domain.blockPeek('/dev/sda', 0, 1024, [], function(err, res) {
        expect(err).to.exist;
        expect(err.code).to.be.equal(libvirt.VIR_ERR_NO_SUPPORT);

        // NOTE: not supported by test driver
        //expect(err).to.not.exist;
        //expect(res).to.be.instanceof(Buffer);

        done();
      });
    });

    it('should take, lookup, revert and delete a domain snapshot', function(done) {
      if (semver.lt(test.version, '1.0.0')) { return done(); }

      var xml = h.fixture('snapshot.xml');
      test.domain.takeSnapshot(xml, [], function(err) {
        expect(err).to.not.exist;

        test.domain.lookupSnapshotByName('test-snapshot', function(err, res) {
          expect(err).to.not.exist;
          expect(xml).to.match(/<name>test-snapshot<\/name>/);

          test.domain.revertToSnapshot('test-snapshot', function(err) {
            expect(err).to.not.exist;

            test.domain.deleteSnapshot('test-snapshot', function(err) {
              expect(err).to.not.exist;
              done();
            });
          });
        });
      });
    });


    // it('should register, test and unregister a domain event', function(done) {
    //   libvirt.setupEvent();
    //   var events = [];
    //   var callbackid;

    //   test.hypervisor.registerDomainEvent({
    //     evtype: test.hypervisor.VIR_DOMAIN_EVENT_ID_LIFECYCLE,
    //     callback: function(hv, dom, data) {
    //       events.push(data.evtype);

    //       if (events.length === 2) {
    //         expect(events).to.eql([5, 2]);
    //         test.hypervisor.unregisterDomainEvent(callbackid, function(err, result) {
    //           expect(err).to.not.exist;
    //           expect(result).to.be.true;
    //           done();
    //         });
    //       }
    //     }
    //   }, function(err, result) {
    //     expect(err).to.not.exist;
    //     callbackid = result;

    //     test.domain.shutdown(function(err, result) {
    //       expect(err).to.not.exist;
    //       test.domain.start(function(err, result) {
    //         expect(err).to.not.exist;
    //       });
    //     });
    //   });
    // });


  });

  describe('accessors/mutators', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;

        test.hypervisor.lookupDomainById(1, function(err, domain) {
          expect(err).to.not.exist;
          expect(domain).to.exist;
          expect(domain._parent).to.exist;
          test.domain = domain;
          done();
        });
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        done();
      });
    });

    it('should return the id', function(done) {
      test.domain.getId(function(err, id) {
        expect(err).to.not.exist;
        expect(id).to.equal(1);
        done();
      });
    });

    it('should return the operating system type', function(done) {
      test.domain.getOSType(function(err, osType) {
        expect(err).to.not.exist;
        expect(osType).to.equal('linux');
        done();
      });
    });

    it('should return the name', function(done) {
      test.domain.getName(function(err, name) {
        expect(err).to.not.exist;
        expect(name).to.equal('test');
        done();
      });
    });

    it('should return the uuid', function(done) {
      test.domain.getUUID(function(err, uuid) {
        expect(err).to.not.exist;
        expect(uuid).to.exist;
        done();
      });
    });

    it('should return the domain information', function(done) {
      test.domain.getInfo(function(err, info) {
        expect(err).to.not.exist;
        expect(info).to.include({
          state: 'running',
          maxMemory: 8388608,
          memory: 2097152,
          vcpus: 2
        });
        expect(info.cpuTime).to.exist;

        done();
      });
    });

    it('should indicate if autostart is enabled', function(done) {
      test.domain.getAutostart(function(err, autoStart) {
        expect(err).to.not.exist;
        expect(autoStart).to.equal(false);
        done();
      });
    });

    it('should enable or disable autostart', function(done) {
      test.domain.setAutostart(false, function(err, result1) {
        expect(err).to.not.exist;
        expect(result1).to.be.true;

        test.domain.getAutostart(function(err, result2) {
          expect(err).to.not.exist;
          expect(result2).to.be.false;

          test.domain.setAutostart(true, function(err, result3) {
            expect(err).to.not.exist;
            expect(result3).to.be.true;

            test.domain.getAutostart(function(err, result4) {
              expect(err).to.not.exist;
              expect(result4).to.be.true;

              done();
            });
          });
        });
      });
    });

    it('should return maximum amount of physical memory allocated to a domain', function(done) {
      test.domain.getMaxMemory(function(err, memory) {
        expect(memory).to.equal(8388608);
        done();
      });
    });

    it('should change the maximum amount of physical memory allocated to a domain', function(done) {
      test.domain.setMaxMemory(512000, function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;

        test.domain.getMaxMemory(function(err, memory) {
          expect(err).to.not.exist;
          expect(memory).to.equal(512000);
          done();
        });
      });
    });

    it('should dynamically change the runtime amount of memory allocated to a domain', function(done) {
      test.domain.setMemory(256000, function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;

        test.domain.getInfo(function(err, info) {
          expect(err).to.not.exist;
          expect(info.memory).to.equal(256000);
          done();
        });
      });
    });

    it('should return the maximum number of virtual CPUs supported for the guest VM', function(done) {
      test.domain.getMaxVcpus(function(err, maxVcpus) {
        expect(err).to.not.exist;
        expect(maxVcpus).to.equal(2);
        done();
      });
    });

    it('should indicate whether the domain is active', function(done) {
      test.domain.isActive(function(err, active) {
        expect(err).to.not.exist;
        expect(active).to.equal(true);
        done();
      });
    });

    it('should indicate whether the domain is persistent', function(done) {
      test.domain.isPersistent(function(err, persistent) {
        expect(err).to.not.exist;
        expect(persistent).to.equal(true);
        done();
      });
    });

    it('should indicate whether the domain has been updated', function(done) {
      test.domain.isUpdated(function(err, updated) {
        expect(err).to.not.exist;
        expect(updated).to.equal(false);
        done();
      });
    });

    it('should return domain xml representation', function(done) {
      test.domain.toXml(function(err, xml) {
        expect(err).to.not.exist;
        expect(xml).to.match(/<name>test<\/name>/);
        done();
      });
    });

    it('should return basic information about a domain\'s block device', function(done) {
      test.domain.getBlockInfo('/path', function(err, info) {
        // NOTE: not supported on test-driver
        expect(err).to.exist;

        // expect(err).to.not.exist;
        // expect(info.capacity).to.exist;
        // expect(info.allocation).to.exist;
        // expect(info.physical).to.exist;
        done();
      });
    });

    it('should return block device stats for block devices attached to the domain', function(done) {
      test.domain.getBlockStats('/dev/sda', function(err, stats) {
        expect(err).to.exist;

        // @todo find a valid path to test...
        // expect(err).to.not.exist;
        // expect(stats.read_requests).to.exist;
        // expect(stats.read_bytes).to.exist;
        // expect(stats.write_requests).to.exist;
        // expect(stats.write_bytes).to.exist;
        done();
      });
    });

    it('should get the domain scheduler type', function(done) {
      test.domain.getSchedulerType(function(err, type) {
        expect(err).to.not.exist;
        expect(type).to.equal('fair');
        done();
      });
    });

    it('should get the domain scheduler parameters', function(done) {
      // test driver always return 50 as weight
      // and it doesn't set new values for weight
      test.domain.getSchedulerParameters(function(err, params) {
        expect(err).to.not.exist;
        expect(params.weight).to.equal(50);
        done();
      });
    });

    it('should return the domain security labels', function(done) {
      test.domain.getSecurityLabel(function(err, label) {
        expect(err).to.exist;

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // console.log(label);
        done();
      });
    });

    it('should return whether the domain has a managed save image', function(done) {
      if (semver.lt(test.version, '1.0.0')) { return done(); }

      test.domain.hasManagedSaveImage(function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.false;
        done();
      });
    });

    it('should return network interface statistics of the domain', function(done) {
      test.domain.getInterfaceStats('eth0', function(err, stats) {
        expect(err).to.exist;

        // @todo attach network device to domain, then check
        // expect(err).to.not.exist;
        // expect(stats.rx.bytes).to.exist;
        // expect(stats.rx.packets).to.exist;
        // expect(stats.rx.errors).to.exist;
        // expect(stats.rx.drop).to.exist;
        // expect(stats.tx.bytes).to.exist;
        // expect(stats.tx.packets).to.exist;
        // expect(stats.tx.errors).to.exist;
        // expect(stats.tx.drop).to.exist;

        done();
      });
    });

    it('should return information about progress of a background job on a domain', function(done) {
      test.domain.getJobInfo(function(err, info) {
        expect(err).to.exist;

        // NOTE: not supported on test driver
        // expect(err).to.not.exist;
        // expect(info.type).to.exist;
        // expect(info.time.elapsed).to.exist;
        // expect(info.time.remaining).to.exist;
        // expect(info.data.total).to.exist;
        // expect(info.data.processed).to.exist;
        // expect(info.data.remaining).to.exist;
        // expect(info.memory.total).to.exist;
        // expect(info.memory.processed).to.exist;
        // expect(info.memory.remaining).to.exist;
        // expect(info.file.total).to.exist;
        // expect(info.file.processed).to.exist;
        // expect(info.file.remaining).to.exist;

        done();
      });
    });

    it('should return domain\'s memory statistics', function(done) {
      test.domain.getMemoryStats(function(err, stats) {
        expect(err).to.exist;

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // expect(stats.swapIn).to.exist;
        // expect(stats.swapOut).to.exist;
        // expect(stats.majorFault).to.exist;
        // expect(stats.minorFault).to.exist;
        // expect(stats.unused).to.exist;
        // expect(stats.available).to.exist;

        done();
      });
    });

    it('should get information about vcpus', function(done) {
      test.domain.getVcpus(function(err, vcpus) {
        expect(err).to.not.exist;
        expect(vcpus).to.exist;

        expect(vcpus).to.be.instanceOf(Array);
        expect(vcpus[0].number).to.exist;
        expect(vcpus[0].state).to.exist;
        expect(vcpus[0].state).to.equal('running');
        expect(vcpus[0].cpuTime).to.exist;
        expect(vcpus[0].cpu).to.exist;
        expect(vcpus[0].affinity).to.exist;

        var affinity = vcpus[0].affinity;
        var real_cpu = 0; //pedagogical purpose
        expect(affinity[real_cpu].usable).to.exist;
        done();
      });
    });

    it('should dynamically change the number of virtual CPUs used by the domain', function(done) {
      test.domain.setVcpus(1, function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.be.true;
        done();
      });
    });

    it('should show if the domain has a current snapshot', function(done) {
      if (semver.lt(test.version, '1.0.0')) { return done(); }

      test.domain.hasCurrentSnapshot(function(err, res) {
        expect(err).to.not.exist;
        expect(res).to.be.false;
        done();
      });
    });

    it('should return information about the current domain snapshot', function(done) {
      if (semver.lt(test.version, '1.0.0')) { return done(); }

      var xml = h.fixture('snapshot.xml');
      test.domain.takeSnapshot(xml, [], function(err) {
        expect(err).to.not.exist;

        test.domain.hasCurrentSnapshot(function(err, res) {
          expect(err).to.not.exist;
          expect(res).to.be.true;

          test.domain.getCurrentSnapshot(function(err, snapshot) {
            expect(err).to.not.exist;
            expect(snapshot).to.match(/<name>test-snapshot<\/name>/);

            test.domain.deleteSnapshot('test-snapshot', function(err) {
              expect(err).to.not.exist;
              done();
            });
          });
        });
      });
    });

    it('should set a maximum tolerable time for which the domain is allowed to be paused at the end of live migration', function(done) {
      //test driver doesn't support this function
      //Milliseconds
      test.domain.setMigrationMaxDowntime(100000, function(err, result) {
        expect(err.code).to.equal(libvirt.VIR_ERR_NO_SUPPORT);

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // expect(result).to.be.true;

        done();
      });
    });

    it('should return all the domain snapshots', function(done) {
      if (semver.lt(test.version, '1.0.0')) { return done(); }

      var xml = h.fixture('snapshot.xml');
      test.domain.takeSnapshot(xml, [], function(err) {
        expect(err).to.not.exist;

        test.domain.getSnapshots(function(err, snapshots) {
          expect(err).to.not.exist;
          expect(snapshots).to.be.instanceOf(Array);
          expect(snapshots[0]).to.exist;
          expect(snapshots[0]).to.match(/<name>test-snapshot<\/name>/);

          test.domain.deleteSnapshot('test-snapshot', function(err) {
            expect(err).to.not.exist;
            done();
          });
        });
      });
    });
  });

});

/*

  it('should set the domain scheduler parameters', function() {
    try {
      var info = domain.getSecurityLabel();
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
    //info.label.should_not_be undefined
    //info.enforcing.should_not_be undefined
  });

});
*/
