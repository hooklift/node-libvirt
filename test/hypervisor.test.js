'use strict';

var libvirt = require('../lib'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    h = require('./lib/helper'),
    semver = require('semver'),
    expect = require('chai').expect;

var test = {};
describe('Hypervisor', function() {
  before(function() {
    SegfaultHandler.registerHandler();
    return h.getLibVirtVersion()
      .then(function(version) {
        test.version = version;
      });
  });

  describe('construction', function() {
    it('should create a hypervisor object', function() {
      var test = new Hypervisor('test:///default');
      expect(test.uri).to.equal('test:///default');
      expect(test.username).to.exist;
      expect(test.password).to.exist;
      expect(test.readOnly).to.exist;
    });

    it('should open a connection', function(done) {
      var connection = new Hypervisor('test:///default');
      connection.connect(function(err) {
        expect(err).to.not.exist;

        connection.disconnect(function(err) {
          expect(err).to.not.exist;
          done();
        });
      });
    });

    it('should open a read-only connection', function(done) {
      var connection = new Hypervisor('test:///default', true);
      connection.connect(function(err) {
        expect(err).to.not.exist;

        connection.disconnect(function(err) {
          expect(err).to.not.exist;
          done();
        });
      });
    });

    // it('should not return the version for a read-only conn', function() {
    //   //the test driver sends the version in read-only!!
    //   //this is not a big deal so we'll let it pass
    //   var readonlyConn = new Hypervisor('test:///default', true);
    //   var version = readonlyConn.getVersion();
    //   expect(version).to.not.be.null;
    // });

    // it('should open an authenticated connection': function(beforeExit, assert) {
    //   // var hypervisor2 = new libvirt.Hypervisor('esx://172.16.103.128/?no_verify=1', {
    //   //     username: 'myusername',
    //   //     password: 'mypassword',
    //   //     readOnly: false
    //   // });

    //   // assert.isDefined(hypervisor2);
    //   // assert.isNotNull(hypervisor2);
    //   // assert.isNotNull(hypervisor2.getVersion());
    // });
  });

  describe('methods', function() {
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


    [
      { name: 'capabilities', method: 'getCapabilities' },
      { name: 'host name', method: 'getHostname' },
      { name: 'type', method: 'getType' },
      { name: 'connection uri', method: 'getConnectionUri' },
      { name: 'libvirt version', method: 'getLibVirtVersion' },
      { name: 'version', method: 'getVersion' },
      { name: 'connection secure', method: 'isConnectionSecure' },
      { name: 'connection encrypted', method: 'isConnectionEncrypted' },
      { name: 'connection alive', method: 'isConnectionAlive'},

      // NOTE: not supported by test driver:
      { name: 'system info', method: 'getSysInfo', disabled: true },
    ].forEach(function(testCase) {
      it('should return hypervisor ' + testCase.name, function(done) {
        test.hypervisor[testCase.method](function(err, result) {
          if (!!testCase.disabled) {
            expect(err).to.exist;
            expect(result).to.not.exist;
          } else {
            expect(err).to.not.exist;
            expect(result).to.exist;
          }

          done();
        });
      });
    });
    it('should return memory details for hypervisor', function(done) {
      test.hypervisor.getNodeMemoryStats(libvirt.VIR_NODE_MEMORY_STATS_ALL_CELLS,
        0, function(err, result) {
        // Not supported by test driver
        done();
      });
    });
    it('should return the vcpu maximum number supported for a guest VM', function(done) {
      test.hypervisor.getMaxVcpus('kvm', function(err, result) {
        expect(err).to.be.null;
        expect(result).to.not.be.null;
        done();
      });
    });

    it('should allow setting the keep alive interval and count', function(done) {
      // @todo not supported with test driver
      test.hypervisor.setKeepAlive(10, 10, function(err, result) {
        expect(err).to.exist;
        expect(result).to.not.be.ok;
        done();
      });
    });


    [
      {
        name: 'defined but inactive domains',
        method: 'listDefinedDomains', expected: []
      },
      {
        name: 'defined but inactive networks',
        method: 'listDefinedNetworks', expected: []
      },
      {
        name: 'defined but inactive storage pools',
        method: 'listDefinedStoragePools',
        expected: []
      },
      {
        name: 'defined but inactive interfaces',
        method: 'listDefinedInterfaces', expected: []
      },
      {
        name: 'active physical host interfaces',
        method: 'listActiveInterfaces', expected: ['eth1']
      },
      {
        name: 'active networks',
        method: 'listActiveNetworks', expected: ['default']
      },
      {
        name: 'active storage pools',
        method: 'listActiveStoragePools', expected: ['default-pool']
      },
      {
        name: 'network filters',
        method: 'listNetworkFilters', disabled: true
      },
      {
        name: 'defined secrets (uuids)',
        method: 'listSecrets', disabled: true
      }

    ].forEach(function(testCase) {
      it('should list names of ' + testCase.name, function(done) {
        test.hypervisor[testCase.method](function(err, result) {
          if (!!testCase.disabled) {
            expect(err).to.exist;
            expect(result).to.not.exist;
          } else {
            expect(err).to.not.exist;
            expect(result).to.be.instanceOf(Array);
            expect(result).to.eql(testCase.expected);
          }

          done();
        });
      });
    });

    it('should compute the most feature-rich CPU', function(done) {
      if (semver.lt(test.version, '1.0.0')) { return done(); }

      var cpu1 = h.fixture('cpu1.xml');
      var cpu2 = h.fixture('cpu2.xml');
      var computed_cpu = h.fixture('match_bt_cpu1_and_cpu2.xml');
      var xmlCPUs = [cpu1, cpu2];

      test.hypervisor.getBaselineCPU(xmlCPUs, function(err, cpu) {
        expect(err).to.not.exist;
        expect(cpu).to.exist;
        done();
      });
    });

    it('should compare given cpu description with host CPU', function(done) {
      var cpu = h.fixture('cpu1.xml');

      // NOTE: not supported by test driver
      test.hypervisor.compareCPU(cpu, function(err, result) {
        expect(err).to.exist;
        expect(result).to.not.exist;
        done();
      });
    });

    it('should list active domains ids', function(done) {
      // NOTE: 1 is the default active domain the test driver
      test.hypervisor.listActiveDomains(function(err, domains) {
        expect(err).to.not.exist;
        expect(domains).to.eql([1]);
        done();
      });
    });

    [
      {
        name: 'defined but inactive domains',
        method: 'getNumberOfDefinedDomains', expected: 0
      },
      {
        name: 'defined but inactive networks',
        method: 'getNumberOfDefinedNetworks', expected: 0
      },
      {
        name: 'defined but inactive storage pools',
        method: 'getNumberOfDefinedStoragePools', expected: 0
      },
      {
        name: 'active domains',
        method: 'getNumberOfActiveDomains', expected: 1
      },
      {
        name: 'active networks',
        method: 'getNumberOfActiveNetworks', expected: 1
      },
      {
        name: 'active physical host interfaces',
        method: 'getNumberOfActiveInterfaces', expected: 1
      },
      {
        name: 'active storage pools',
        method: 'getNumberOfActiveStoragePools', expected: 1
      },
      {
        name: 'currently defined secrets',
        method: 'getNumberOfActiveStoragePools', expected: 1
      },
      {
        name: 'network filters',
        method: 'getNumberOfNetworkFilters', disabled: true
      },
      {
        name: 'currently defined secrets',
        method: 'getNumberOfSecrets', disabled: true
      }
    ].forEach(function(testCase) {
      it('should return the number of ' + testCase.name, function(done) {
        test.hypervisor[testCase.method](function(err, count) {
          if (!!testCase.disabled) {
            expect(err).to.exist;
            expect(count).to.not.exist;
          } else {
            expect(err).to.not.exist;
            expect(count).to.equal(testCase.expected);
          }

          done();
        });
      });
    });

  });

  describe('node', function() {
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

    it('should list node devices names', function(done) {
      test.hypervisor.listNodeDevices(function(err, devices) {
        expect(err).to.not.exist;
        expect(devices).to.exist;
        expect(devices).to.be.instanceOf(Array);
        done();
      });
    });

    it('should return the node security model', function(done) {
      test.hypervisor.getNodeSecurityModel(function(err, security) {
        expect(err).to.exist;
        expect(security).to.not.exist;

        // NOTE: not supported by current driver
        // expect(err).to.not.exist;
        // espect(security).to.exist;
        // expect(security.model).to.exist;
        // expect(security.doi).to.exist;
        done();
      });
    });


    it('should return the node information where hypervisor is running', function(done) {
      test.hypervisor.getNodeInfo(function(err, info) {
        expect(err).to.not.exist;
        expect(info).to.exist;
        expect(info).to.eql({
          model: 'i686',
          memory: 3145728,
          cpus: 16,
          mhz: 1400,
          nodes: 2,
          sockets: 2,
          cores: 2,
          threads: 2
        });

        done();
      });
    });

    it('should return free memory of the physical node', function(done) {
      test.hypervisor.getNodeFreeMemory(function(err, result) {
        // NOTE: unsupported by test driver
        expect(err).to.exist;
        expect(result).to.not.exist;

        // expect(err).to.not.exist;
        // expect(result).to.exist;
        done();
      });
    });

    it('should return the amount of node free memory in one or more NUMA cells', function(done) {
      var startCell = 0;
      var maxCells = 2;
      test.hypervisor.getNodeCellsFreeMemory(startCell, maxCells, function(err, result) {
        expect(err).to.not.exist;
        expect(result).to.exist;
        expect(result).to.eql([ 2097152, 4194304 ]);
        done();
      });
    });

  });

  // it('should register function callbacks for domain events', function(done) {
  //   test.hypervisor.lookupDomainByName('test', function(err, hypervisor) {
  //     expect(err).to.not.exist;
  //     expect(hypervisor).to.be.ok;

  //     var args = {
  //       evtype: test.hypervisor.VIR_DOMAIN_EVENT_ID_LIFECYCLE,
  //       domain: domain,
  //       callback: function(hyp, dom, data) {
  //         expect(data.evtype).to.equal(hyp.VIR_DOMAIN_EVENT_ID_LIFECYCLE);
  //         expect(data.detail).to.equal(dom.VIR_DOMAIN_EVENT_STOPPED_SHUTDOWN);

  //         dom.getName(function(err, name) {
  //           expect(name).to.equal('test');
  //           done();
  //         });
  //       }
  //     };

  //     callback_id = test.hypervisor.registerDomainEvent(args);
  //     domain.shutdown();
  //     assert.eql(hypervisor.unregisterDomainEvent(callback_id), true);
  //   });
  // });

  // 'should unregister callbacks listening for domain events': function(beforeExit, assert) {
  //     var args = { evtype: hypervisor.VIR_DOMAIN_EVENT_ID_LIFECYCLE,
  //                  callback: function(hyp, dom, evtype, detail) {}
  //                 };

  //     callback_id = hypervisor.registerDomainEvent(args);
  //     assert.eql(hypervisor.unregisterDomainEvent(callback_id), true);
  // },


});
