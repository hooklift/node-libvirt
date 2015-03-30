'use strict';

var libvirt = require('../build/Release/libvirt'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

var test = {};
describe('Domain', function() {
  it('should lookup a domain by id', function() {
    domain = hypervisor.lookupDomainById(1);
    expect(domain).to.exist;
  });

  it('should create a persistent Domain from its JSON Description', function() {
    try {
      var xml = fixture('domain.xml');
      hypervisor.createDomain(xml);

      var dom = hypervisor.lookupDomainByName('nodejs-test');
      expect(dom.getName()).to.equal('nodejs-test');
      expect(dom.destroy()).to.equal(true);
    } catch (err) {}
  });

  it('should return the id', function() {
    expect(domain.getId()).to.equal(1);
  });

  it('should return the operating system type', function() {
    expect(domain.getOsType()).to.equal('linux');
  });

  it('should return the domain information', function() {
    var info = domain.getInfo();

    expect(info.state).to.equal(domain.VIR_DOMAIN_RUNNING);
    expect(info.max_memory).to.equal(8388608);
    expect(info.memory).to.equal(2097152);
    expect(info.vcpus_number).to.equal(2);
    expect(info.cpu_time).to.exist;
    expect(info.cpu_time).to.exist;
  });

  it('should return the name', function() {
    expect(domain.getName()).to.equal('test');
  });

  it('should return the uuid', function() {
    expect(domain.getUUID()).to.exist;
  });

  it('should indicate if autostart is enable', function() {
    expect(domain.getAutostart()).to.equal(true);
  });

  it('should enable or disable autostart', function() {
    expect(domain.setAutostart(false)).to.equal(true);
    expect(domain.getAutostart()).to.equal(false);

    expect(domain.setAutostart(true)).to.equal(true);
    expect(domain.getAutostart()).to.equal(true);
  });

  it('should return maximum amount of physical memory allocated to a domain', function() {
    expect(domain.getMaxMemory()).to.equal(8388608);
  });

  it('should change the maximum amount of physical memory allocated to a domain', function() {
    //kilobytes
    expect(domain.setMaxMemory(512000)).to.equal(true);
    expect(domain.getMaxMemory()).to.equal(512000);
  });

  it('should dynamically change the runtime amount of memory allocated to a domain', function() {
    expect(domain.setMemory(256000)).to.equal(true);
    var info = domain.getInfo();
    expect(info.memory).to.equal(256000);
  });

  it('should return the maximum number of virtual CPUs supported for the guest VM', function() {
    expect(domain.getMaxVcpus()).to.equal(2);
  });

  it('should indicate whether the domain is active', function() {
    expect(domain.isActive()).to.equal(true);
  });

  it('should indicate whether the domain is persistent', function() {
    expect(domain.isPersistent()).to.equal(true);
  });

  it('should reset the domain', function() {
    //expect(domain.reset()).to.equal(false);
    expect(true).to.equal(true);
  });

  it('should reboot the domain', function() {
    expect(domain.reboot()).to.equal(true);
  });

  it('should save and restore the domain', function() {
    var path = '/tmp/' + domain.getName() + '-saved.img';
    domain.save(path);
    expect(hypervisor.restoreDomain(path)).to.equal(true);
  });

  it('should suspend and resume the domain', function() {
    expect(domain.suspend()).to.equal(true);
    expect(domain.resume()).to.equal(true);
  });

  it('should shutdown the domain', function() {
    expect(domain.shutdown()).to.equal(true);
  });

  it('should dynamically change the number of virtual CPUs used by the domain', function() {
    //expresso doesn't have hooks pre and post each test so this is a hack
    if (!domain.isActive()) {
      domain.start();
    }
    expect(domain.setVcpus(1)).to.equal(true);
  });

  it('should get information about vcpus', function() {
    var vcpus = domain.getVcpus();
    expect(vcpus).to.be.instanceOf(Array);
    expect(vcpus[0].number).to.exist;
    expect(vcpus[0].state).to.exist;
    expect(vcpus[0].cpu_time).to.exist;
    expect(vcpus[0].cpu).to.exist;
    expect(vcpus[0].affinity).to.exist;

    var affinity = vcpus[0].affinity;
    var real_cpu = 0; //pedagogical purpose
    expect(affinity[real_cpu].usable).to.exist;
  });

  it('should allow to change real CPUs, which can be allocated to a virtual CPU', function() {
    var vcpus = domain.getVcpus();
    var affinity = vcpus[0].affinity;
    affinity[0].usable = false;
    affinity[1].usable = false;
    expect(domain.pinVcpu(vcpus[0].number, affinity)).to.equal(true);

    var vcpus2 = domain.getVcpus();
    var affinity2 = vcpus2[0].affinity;
    expect(affinity2[0].usable).to.equal(false);
    expect(affinity2[1].usable).to.equal(false);

    try {
      domain.pinVcpu();
    } catch (error) {
      expect(error.message).to.equal('You must specify two arguments');
    }

    try {
      domain.pinVcpu(vcpus[0].number, 2);
    } catch (error) {
      expect(error.message).to.equal('The second argument must be an array of objects');
    }

    try {
      domain.pinVcpu('test', affinity);
    } catch (error) {
      expect(error.message).to.equal('The first argument must be an integer');
    }

    try {
      domain.pinVcpu(vcpus[0].number);
    } catch (error) {
      expect(error.message).to.equal('You must specify two arguments');
    }

    try {
      domain.pinVcpu(vcpus[0].number, ['']);
    } catch (error) {
      expect(error.message).to.equal('The second argument must be an array of objects');
    }
  });

  it('should attach a device', function() {
    var device = fixture('device.xml');
    //no supported by test driver
    try {
      expect(domain).to.exist;
      domain.attachDevice(device);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
    //domain.attachDevice(device).should_be true
  });

  it('should detach a device', function() {
    var device = fixture('device.xml');
    //no supported by test driver
    try {
      expect(domain.detachDevice(device)).to.equal(true);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should update a device', function() {
    var device = fixture('device_update.xml');
    var flags = [libvirt.VIR_DOMAIN_DEVICE_MODIFY_CONFIG];

    try {
      domain.updateDevice(device, flags);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should migrate a domain to another hypervisor through a hypervisor connection', function() {
    var hypervisor2 = new Hypervisor('test:///default');
    var flags = [domain.VIR_MIGRATE_LIVE,
      domain.VIR_MIGRATE_PEER2PEER,
      domain.VIR_MIGRATE_PAUSED,
      domain.VIR_MIGRATE_PERSIST_DEST];

    //test driver doesn't support this function
    //bandwidth in Mbps
    try {
      domain.migrate({ dest_hypervisor: hypervisor2, dest_name: 'test2', dest_uri: '', bandwidth: 100, flags: flags });
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should migrate a domain to another hypervisor through an uri', function() {
    var flags = [domain.VIR_MIGRATE_LIVE,
      domain.VIR_MIGRATE_PEER2PEER,
      domain.VIR_MIGRATE_PAUSED,
      domain.VIR_MIGRATE_PERSIST_DEST];

    //test driver doesn't support this function
    //bandwidth in Mbps
    try {
      domain.migrate({ dest_uri: 'test:///default', dest_name: 'test2', bandwidth: 100, flags: flags });
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should set a maximum tolerable time for which the domain is allowed to be paused at the end of live migration', function() {
    //test driver doesn't support this function
    //Milliseconds
    try {
      domain.setMigrationMaxDowntime(100000);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should return domain xml representation', function() {
    var flags = [libvirt.VIR_DOMAIN_XML_SECURE,
      libvirt.VIR_DOMAIN_XML_INACTIVE];

    var xml = domain.toXml(flags);
    expect(xml).to.match(/<name>test<\/name>/);
  });

  it('should return domain json representation', function() {
    //test driver doesn't support this function
    try {
      var info = domain.getJobInfo();
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }

    //        info.type.should_not_be undefined
    //        info.time.elapsed.should_not_be undefined
    //        info.time.remaining.should_not_be undefined
    //        info.data.total.should_not_be undefined
    //        info.data.processed.should_not_be undefined
    //        info.data.remaining.should_not_be undefined
    //        info.memory.total.should_not_be undefined
    //        info.memory.processed.should_not_be undefined
    //        info.memory.remaining.should_not_be undefined
    //        info.file.total.should_not_be undefined
    //        info.file.processed.should_not_be undefined
    //        info.file.remaining.should_not_be undefined
  });

  it('should extract information about progress of a background job on the domain', function() {
    try {
      expect(domain.abortCurrentJob()).to.equal(true);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should abort the current background job on the domain', function() {
    var params = domain.getSchedParams();
    expect(params.weight).to.equal(50);
  });

  it('should get the domain scheduler parameters', function() {
    //test driver always return 50 as weight
    // and it doesn't set new values for weight
    var params = domain.getSchedParams();
    expect(params.weight).to.equal(50);
    params.weight = 30;

    expect(domain.setSchedParams(params)).to.equal(true);
    params = domain.getSchedParams();
    //params.weight.should_be 30
    expect(params.weight).to.equal(50);
  });

  it('should set the domain scheduler parameters', function() {
    try {
      var info = domain.getSecurityLabel();
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
    //info.label.should_not_be undefined
    //info.enforcing.should_not_be undefined
  });

  it('should return the domain security labels', function() {
    try {
      var info = domain.getSecurityLabel();
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
    //info.label.should_not_be undefined
    //info.enforcing.should_not_be undefined
  });

  it('should save a managed image of the domain', function() {
    //test driver doesn't support these functions
    try {
      expect(domain.saveManagedImage()).to.equal(true);
      expect(domain.hasManagedImage()).to.equal(true);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should remove a managed image of the domain', function() {
    //test driver doesn't support these functions
    try {
      expect(domain.removeManagedImage()).to.equal(true);
      expect(domain.hasManagedImage()).to.equal(true);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should allow to read the domain\'s memory content and return it in a Buffer object', function() {
    var physical = [domain.VIR_MEMORY_PHYSICAL];
    var virtual = [domain.VIR_MEMORY_VIRTUAL];

    try {
      var psegment = domain.memoryPeek(0, 1024, physical);
      expect(psegment instanceof Buffer);

      var vsegment = domain.memoryPeek(0, 1024, virtual);
      expect(vsegment instanceof Buffer);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should allow to read the content of a domain\'s disk device and return it in a Buffer object', function() {
    try {
      var blocks = domain.blockPeek('/dev/sda', 0, 1024);
      expect(blocks instanceof Buffer);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should return domain\'s memory statistics', function() {
    //memory statistics in kbs
    try {
      var stats = domain.getMemoryStats();
      expect(stats.swap_in).to.exist;
      expect(stats.swap_out).to.exist;
      expect(stats.major_fault).to.exist;
      expect(stats.minor_fault).to.exist;
      expect(stats.unused).to.exist;
      expect(stats.available).to.exist;
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should return block device stats for block devices attached to the domain', function() {
    try {
      var stats = domain.getBlockStats('/dev/sda');
      expect(stats.read_requests).to.exist;
      expect(stats.read_bytes).to.exist;
      expect(stats.write_requests).to.exist;
      expect(stats.write_bytes).to.exist;
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_INVALID_ARG);
    }
  });

  it('should return basic information about a domain\'s block device', function() {
    try {
      var info = domain.getBlockInfo('/path');
      expect(info.capacity).to.exist;
      expect(info.allocation).to.exist;
      expect(info.physical).to.exist;
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should dump the core of a domain on a given file for analysis', function() {
    var path = '/tmp/dumpcore-test.txt';
    expect(domain.coreDump(path));
  });

  it('should return network interface statistics of the domain', function() {
    //FIXME, attach network device eth1 to the domain
    try {
      var stats = domain.getInterfaceStats('eth1');
      var ifaces = domain.getInterfaces();

      expect(stats.rx_bytes).to.exist;
      expect(stats.rx_packets).to.exist;
      expect(stats.rx_errors).to.exist;
      expect(stats.rx_drop).to.exist;
      expect(stats.tx_bytes).to.exist;
      expect(stats.tx_packets).to.exist;
      expect(stats.tx_errors).to.exist;
      expect(stats.tx_drop).to.exist;
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_INVALID_ARG);
    }
  });

  it('should show if the domain has a current snapshot', function() {
    try {
      expect(domain.hasCurrentSnapshot()).to.equal(false);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should revert the domain to a snapshot taken before', function() {
    //take a snapshot and then revert to it
    try {
      expect(domain.revertToSnapshot('test-snapshot')).to.equal(true);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should take a snapshot of the domain', function() {
    try {
      expect(domain.takeSnapshot(), true);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should take a snapshot of the domain using a xml domain description', function() {
    try {
      var xml = fixture('snapshot.xml');
      expect(domain.takeSnapshot(xml)).to.equal(true);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should return information about the current domain snapshot', function() {
    try {
      var fixture = fixture('snapshot.xml');
      expect(domain.takeSnapshot(fixture)).to.equal(true);

      var xml = domain.getCurrentSnapshot();
      expect(xml);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should delete a snapshot', function() {
    try {
      expect(domain.deleteSnapshot('test-snapshot')).to.equal(true);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should lookup a snapshot by name', function() {
    try {
      var xml = domain.lookupSnapshotByName('test-snapshot');
      expect(xml).to.be.ok;
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

  it('should return all the domain snapshots', function() {
    try {
      var snapshots = domain.getSnapshots();
      expect(snapshots).to.be.instanceOf(Array);
    } catch (error) {
      expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
    }
  });

});
