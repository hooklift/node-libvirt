require.paths.unshift('build/default/src');
var sys = require('sys');
var libvirt = require('libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('test:///default');
var domain;

module.exports = {
    'should lookup a domain by id': function(assert) {
        domain = hypervisor.lookupDomainById(1);
        assert.isNotNull(domain);
    },

    'should create a persistent Domain from its JSON Description': function(assert) {
        var xml = fixture('domain.xml');
        hypervisor.createDomain(xml);

        var dom = hypervisor.lookupDomainByName('nodejs-test');
        assert.eql(dom.getName(), 'nodejs-test');
        assert.eql(dom.stop(), true);
    },

    'should return the id': function(assert) {
        assert.eql(domain.getId(), 1);
    },

    'should return the operating system type': function(assert) {
        assert.eql(domain.getOsType(), 'linux');
    },

    'should return the domain information': function(assert) {
        var info = domain.getInfo();

        assert.eql(info.state, domain.VIR_DOMAIN_RUNNING);
        assert.eql(info.max_memory, 8388608);
        assert.eql(info.memory, 2097152);
        assert.eql(info.vcpus_number, 2);
        assert.isDefined(info.cpu_time);
        assert.ok(info.cpu_time);
    },

    'should return the name': function(assert) {
        assert.eql(domain.getName(), 'test');
    },

    'should return the uuid': function(assert) {
        assert.isNotNull(domain.getUUID());
    },

    'should indicate if autostart is enable': function(assert) {
        assert.eql(domain.getAutostart(), true);
    },

    'should enable or disable autostart': function(assert) {
        assert.eql(domain.setAutostart(false), true);
        assert.eql(domain.getAutostart(), false);

        assert.eql(domain.setAutostart(true), true);
        assert.eql(domain.getAutostart(), true);
    },

    'should return maximum amount of physical memory allocated to a domain': function(assert) {
        assert.eql(domain.getMaxMemory(), 8388608);
    },

    'should change the maximum amount of physical memory allocated to a domain': function(assert) {
        //kilobytes
        assert.eql(domain.setMaxMemory(512000), true);
        assert.eql(domain.getMaxMemory(), 512000);
    },

    'should dynamically change the runtime amount of memory allocated to a domain': function(assert) {
        assert.eql(domain.setMemory(256000), true);
        var info = domain.getInfo();
        assert.eql(info.memory, 256000);
    },

    'should return the maximum number of virtual CPUs supported for the guest VM': function(assert) {
        assert.eql(domain.getMaxVcpus(), 32);
    },

    'should indicate whether the domain is active': function(assert) {
        assert.eql(domain.isActive(), true);
    },

    'should indicate whether the domain is persistent': function(assert) {
        assert.eql(domain.isPersistent(), true);
    },

    'should reboot the domain': function(assert) {
        assert.eql(domain.reboot(), true);
    },

    'should save and restore the domain': function(assert) {
        var path = '/tmp/' + domain.getName() + '-saved.img';
        domain.save(path);
        assert.eql(hypervisor.restoreDomain(path), true);
    },

    'should suspend and resume the domain': function(assert) {
        assert.eql(domain.suspend(), true);
        assert.eql(domain.resume(), true);
    },

    'should shutdown the domain': function(assert) {
        assert.eql(domain.shutdown(), true);
    },

    'should dynamically change the number of virtual CPUs used by the domain': function(assert) {
        //expresso doesn't have hooks pre and post each test so this is a hack
        if(!domain.isActive()) {
            domain.start();
        }
        assert.eql(domain.setVcpus(4), true);
    },

    'should get information about vcpus': function(assert) {
        var vcpus = domain.getVcpus();
        assert.ok(vcpus instanceof Array);
        assert.isDefined(vcpus[0].number);
        assert.isDefined(vcpus[0].state);
        assert.isDefined(vcpus[0].cpu_time);
        assert.isDefined(vcpus[0].cpu);
        assert.isDefined(vcpus[0].affinity);

        var affinity = vcpus[0].affinity;
        var real_cpu = 0; //pedagogical purpose
        assert.isDefined(affinity[real_cpu].usable);
    },

    'should allow to change real CPUs, which can be allocated to a virtual CPU': function(assert) {
        var vcpus = domain.getVcpus();
        var affinity = vcpus[0].affinity;
        affinity[0].usable = false;
        affinity[1].usable = false;
        assert.eql(domain.pinVcpu(vcpus[0].number, affinity), true);

        var vcpus2 = domain.getVcpus();
        var affinity2 = vcpus2[0].affinity;
        assert.eql(affinity2[0].usable, false);
        assert.eql(affinity2[1].usable, false);

        try {
            domain.pinVcpu();
        } catch(error) {
            assert.eql(error.message, 'You must specify two arguments');
        }

        try {
            domain.pinVcpu(vcpus[0].number, 2);
        } catch(error) {
            assert.eql(error.message, 'The second argument must be an array of objects');
        }

        try {
            domain.pinVcpu('test', affinity);
        } catch(error) {
            assert.eql(error.message, 'The first argument must be an integer');
        }

        try {
            domain.pinVcpu(vcpus[0].number);
        } catch(error) {
            assert.eql(error.message, 'You must specify two arguments');
        }

        try {
            domain.pinVcpu(vcpus[0].number, ['']);
        } catch(error) {
            assert.eql(error.message, 'The second argument must be an array of objects');
        }
    },

    'should attach a device': function(assert) {
       var device = fixture('device.xml');
        //no supported by test driver
        try {
            domain.attachDevice(device);
        } catch(error){
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
        //domain.attachDevice(device).should_be true
    },

    'should detach a device': function(assert) {
        var device = fixture('device.xml');
        //no supported by test driver
        try {
            assert.eql(domain.detachDevice(device), true);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should update a device': function(assert) {
        var device = fixture('device_update.xml');
        var flags = [libvirt.VIR_DOMAIN_DEVICE_MODIFY_CONFIG];

        try {
            domain.updateDevice(device, flags);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should migrate a domain to another hypervisor through a hypervisor connection': function(assert) {
        var hypervisor2 = new Hypervisor('test:///default');
        var flags = [ domain.VIR_MIGRATE_LIVE,
                      domain.VIR_MIGRATE_PEER2PEER,
                      domain.VIR_MIGRATE_PAUSED,
                      domain.VIR_MIGRATE_PERSIST_DEST];

        //test driver doesn't support this function
        //bandwidth in Mbps
        try {
            domain.migrate({ dest_hypervisor: hypervisor2, dest_name: 'test2', dest_uri: '', bandwidth: 100, flags: flags });
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should migrate a domain to another hypervisor through an uri': function(assert) {
        var flags = [ domain.VIR_MIGRATE_LIVE,
                      domain.VIR_MIGRATE_PEER2PEER,
                      domain.VIR_MIGRATE_PAUSED,
                      domain.VIR_MIGRATE_PERSIST_DEST];

        //test driver doesn't support this function
        //bandwidth in Mbps
        try {
            domain.migrate({ dest_uri: 'test:///default', dest_name: 'test2', bandwidth: 100, flags: flags });
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should set a maximum tolerable time for which the domain is allowed to be paused at the end of live migration': function(assert) {
      //test driver doesn't support this function
        //Milliseconds
        try {
            domain.setMigrationMaxDowntime(100000);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return domain xml representation': function(assert) {
        var flags = [ libvirt.VIR_DOMAIN_XML_SECURE,
                      libvirt.VIR_DOMAIN_XML_INACTIVE];

        var xml = domain.toXml(flags);
        assert.match(xml, /<name>test<\/name>/);
    },

    'should return domain json representation': function(assert) {
        //test driver doesn't support this function
        try {
            var info = domain.getJobInfo();
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
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
    },

    'should extract information about progress of a background job on the domain': function(assert) {
       try {
            assert.eql(domain.abortCurrentJob(), true);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should abort the current background job on the domain': function(assert) {
        var params = domain.getSchedParams();
        assert.eql(params.weight, 50);
    },

    'should get the domain scheduler parameters': function(assert) {
        //test driver always return 50 as weight
        // and it doesn't set new values for weight
        var params = domain.getSchedParams();
        assert.eql(params.weight, 50);
        params.weight = 30;

        assert.eql(domain.setSchedParams(params), true);
        params = domain.getSchedParams();
        //params.weight.should_be 30
        assert.eql(params.weight, 50);
    },

    'should set the domain scheduler parameters': function(assert) {
        try {
            var info = domain.getSecurityLabel();
        } catch(error) {
           assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
        //info.label.should_not_be undefined
        //info.enforcing.should_not_be undefined
    },

    'should return the domain security labels': function(assert) {
        try {
            var info = domain.getSecurityLabel();
        } catch(error) {
           assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
        //info.label.should_not_be undefined
        //info.enforcing.should_not_be undefined
    },

    'should save a managed image of the domain': function(assert) {
        //test driver doesn't support these functions
        try {
            assert.eql(domain.saveManagedImage(), true);
            assert.eql(domain.hasManagedImage(), true);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should remove a managed image of the domain': function(assert) {
       //test driver doesn't support these functions
        try {
            assert.eql(domain.removeManagedImage(), true);
            assert.eql(domain.hasManagedImage(), true);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should allow to read the domain\'s memory content and return it in a Buffer object': function(assert) {
        var physical = [domain.VIR_MEMORY_PHYSICAL];
        var virtual = [domain.VIR_MEMORY_VIRTUAL];

        try {
            var psegment = domain.memoryPeek(0, 1024, physical);
            assert.ok(psegment instanceof Buffer);

            var vsegment = domain.memoryPeek(0, 1024, virtual);
            assert.ok(vsegment instanceof Buffer);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should allow to read the content of a domain\'s disk device and return it in a Buffer object': function(assert) {
        try {
            var blocks = domain.blockPeek('/dev/sda', 0, 1024);
            assert.ok(blocks instanceof Buffer);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return domain\'s memory statistics': function(assert) {
        //memory statistics in kbs
        try {
            var stats = domain.getMemoryStats();
            assert.isDefined(stats.swap_in);
            assert.isDefined(stats.swap_out);
            assert.isDefined(stats.major_fault);
            assert.isDefined(stats.minor_fault);
            assert.isDefined(stats.unused);
            assert.isDefined(stats.available);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return block device stats for block devices attached to the domain': function(assert) {
        try {
            var stats = domain.getBlockStats('/dev/sda');
            assert.isDefined(stats.read_requests);
            assert.isDefined(stats.read_bytes);
            assert.isDefined(stats.write_requests);
            assert.isDefined(stats.write_bytes);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_INVALID_ARG);
        }
    },

    'should return basic information about a domain\'s block device': function(assert) {
       try {
            var info = domain.getBlockInfo('/path');
            assert.isDefined(info.capacity);
            assert.isDefined(info.allocation);
            assert.isDefined(info.physical);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should dump the core of a domain on a given file for analysis': function(assert) {
        var path = '/tmp/dumpcore-test.txt';
        assert.ok(domain.coreDump(path));
    },

    'should return network interface statistics of the domain': function(assert) {
        //FIXME, attach network device eth1 to the domain
        try {
            var stats = domain.getInterfaceStats('eth1');
            var ifaces = domain.getInterfaces

            assert.isDefined(stats.rx_bytes);
            assert.isDefined(stats.rx_packets);
            assert.isDefined(stats.rx_errors);
            assert.isDefined(stats.rx_drop);
            assert.isDefined(stats.tx_bytes);
            assert.isDefined(stats.tx_packets);
            assert.isDefined(stats.tx_errors);
            assert.isDefined(stats.tx_drop);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_INVALID_ARG);
        }
    },

    'should show if the domain has a current snapshot': function(assert) {
        try {
            assert.eql(domain.hasCurrentSnapshot(), false);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should revert the domain to a snapshot taken before': function(assert) {
       //take a snapshot and then revert to it
        try {
            assert.eql(domain.revertToSnapshot('test-snapshot'), true);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should take a snapshot of the domain': function(assert) {
        try {
            assert.eql(domain.takeSnapshot(), true);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should take a snapshot of the domain using a xml domain description': function(assert) {
       try {
            var xml = fixture('snapshot.xml');
            assert.eql(domain.takeSnapshot(xml), true);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return information about the current domain snapshot': function(assert) {
       try {
            var fixture = fixture('snapshot.xml');
            assert.eql(domain.takeSnapshot(fixture), true);

            var xml = domain.getCurrentSnapshot();
            assert.ok(xml);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should delete a snapshot': function(assert) {
       try {
            assert.eql(domain.deleteSnapshot('test-snapshot'), true);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should lookup a snapshot by name': function(assert) {
        try {
            var xml = domain.lookupSnapshotByName('test-snapshot');
            assert.ok(xml);
        } catch(error) {
           assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return all the domain snapshots': function(assert) {
        try {
            var snapshots = domain.getSnapshots();
            assert.ok(snapshots instanceof Array);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    }
};

