require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

describe 'Domain'
    before
        hypervisor = new Hypervisor('test:///default');
    end

    before_each
        domain = hypervisor.lookupDomainByName('test');
        if(!domain.isActive()) {
            domain.start();
        }
    end

    after_each
        domain.destroy();
    end

    after
        hypervisor.closeConnection();
    end

    it 'should lookup a domain by id'
        var domain_id = hypervisor.lookupDomainById(1);
        domain_id.getId().should_be 1
    end

    it 'should create a persistent Domain from its XML Description'
        var xml = fixture('domain.xml')
        hypervisor = new Hypervisor('test:///default')
        hypervisor.createDomain(xml);

        var dom = hypervisor.lookupDomainByName('libvirt');
        dom.getName().should_be 'libvirt'
        dom.destroy().should_be true
        //dom.getId().should_be undefined
    end

    it 'should return the id'
        domain.getId().should_be 1
    end

    it 'should return the operating system type'
        domain.getOsType().should_be 'linux'
    end

    it 'should return the domain information'
        var info = domain.getInfo();

        info.state.should_be domain.VIR_DOMAIN_RUNNING
        info.max_memory.should_be 8388608
        info.memory.should_be 2097152
        info.vcpus_number.should_be 2
        info.cpu_time.should_not_be undefined
        info.cpu_time.should_not_be 0
    end

    it 'should return the name'
        domain.getName().should_be 'test'
    end

    it 'should return the uuid'
        domain.getUUID().should_not_be null
    end

    it 'should show if autostart is enable'
        domain.getAutostart().should_be true
    end

    it 'should enable or disable autostart'
        domain.setAutostart(false).should_be true
        domain.getAutostart().should_be false

        domain.setAutostart(true).should_be true
        domain.getAutostart().should_be true
    end

    it 'should return maximum amount of physical memory allocated to a domain'
        domain.getMaxMemory().should_be 8388608
    end

    it 'should permit change the maximum amount of physical memory allocated to a domain'
        //kilobytes
        domain.setMaxMemory(512000).should_be true
        domain.getMaxMemory().should_be 512000
    end

    it 'should dynamically change the runtime amount of memory allocated to a domain'
        domain.setMemory(256000).should_be true
        var info = domain.getInfo();
        info.memory.should_be 256000
    end

    it 'should return the maximum number of virtual CPUs supported for the guest VM'
        domain.getMaxVcpus().should_be 32
    end

    it 'should indicate whether the domain is active'
        domain.isActive().should_be true
    end

    it 'should indicate whether the domain is persistent'
        domain.isPersistent().should_be true
    end

    it 'should reboot the domain'
        domain.reboot().should_be true
    end

    it 'should save and restore the domain'
        var path = '/tmp/' + domain.getName() + '-saved.img';
        domain.save(path);
        hypervisor.restoreDomain(path).should_be true
    end

    it 'should suspend and resume the domain'
        domain.suspend().should_be true
        domain.resume().should_be true
    end

    it 'should shutdown the domain'
        domain.shutdown().should_be true
    end

    it 'should dynamically change the number of virtual CPUs used by the domain'
        domain.setVcpus(4).should_be true
    end

    it 'should get information about vcpus'
        var vcpus = domain.getVcpus();
        vcpus.should_be_an_instance_of Array
        vcpus[0].number.should_not_be undefined
        vcpus[0].state.should_not_be undefined
        vcpus[0].cpu_time.should_not_be undefined
        vcpus[0].cpu.should_not_be undefined
        vcpus[0].affinity.should_be_an_instance_of Array

        var affinity = vcpus[0].affinity;
        var real_cpu = 0; //pedagogical purpose
        affinity[real_cpu].usable.should_not_be undefined
    end

    it 'should allow to change real CPUs, which can be allocated to a virtual CPU'
        var vcpus = domain.getVcpus();
        var affinity = vcpus[0].affinity;
        affinity[0].usable = false;
        affinity[1].usable = false;
        domain.pinVcpu(vcpus[0].number, affinity).should_be true

        var vcpus2 = domain.getVcpus();
        var affinity2 = vcpus2[0].affinity;
        affinity2[0].usable.should_be false
        affinity2[1].usable.should_be false

        -{domain.pinVcpu()}.should_throw_error
        -{domain.pinVcpu(vcpus[0].number, 2)}.should_throw_error
        -{domain.pinVcpu('test', affinity)}.should_throw_error
        -{domain.pinVcpu(vcpus[0].number)}.should_throw_error
        -{domain.pinVcpu(vcpus[0].number, [''])}.should_throw_error
    end

    it 'should attach a device'
        var device = fixture('device.xml');
        //no supported by test driver
        try {
            domain.attachDevice(device);
        } catch(error){
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
        //domain.attachDevice(device).should_be true
    end

    it 'should detach a device'
        var device = fixture('device.xml');
        //no supported by test driver
        try {
            domain.detachDevice(device).should_be true
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }

    end

    it 'should update a device'
        var device = fixture('device_update.xml');
        var flags = [libvirt.VIR_DOMAIN_DEVICE_MODIFY_CONFIG];

        try {
            domain.updateDevice(device, flags);
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should migrate a domain to another hypervisor through a hypervisor connection'
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
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should migrate a domain to another hypervisor through an uri'
        var flags = [ domain.VIR_MIGRATE_LIVE,
                      domain.VIR_MIGRATE_PEER2PEER,
                      domain.VIR_MIGRATE_PAUSED,
                      domain.VIR_MIGRATE_PERSIST_DEST];

        //test driver doesn't support this function
        //bandwidth in Mbps
        try {
            domain.migrate({ dest_uri: 'test:///default', dest_name: 'test2', bandwidth: 100, flags: flags });
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should set a maximum tolerable time for which the domain is allowed to be paused at the end of live migration'
        //test driver doesn't support this function
        //Milliseconds
        try {
            domain.setMigrationMaxDowntime(100000);
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should return domain xml representation'
        var flags = [ libvirt.VIR_DOMAIN_XML_SECURE,
                      libvirt.VIR_DOMAIN_XML_INACTIVE];

        var xml = domain.toXml(flags);
        xml.should_match /<name>test<\/name>/
    end

    it 'should extract information about progress of a background job on the domain'
        //test driver doesn't support this function
        try {
            var info = domain.getJobInfo();
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
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
    end

    it 'should abort the current background job on the domain'
        try {
            domain.abortCurrentJob().should_be true
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }

    end

    it 'should get the domain scheduler parameters'
        var params = domain.getSchedParams();
        params.weight.should_be 50
    end

    it 'should set the domain scheduler parameters'
        //test driver has a bug, always return 50 as weight
        // and it doesn't set new values for weight
        var params = domain.getSchedParams();
        params.weight.should_be 50
        params.weight = 30;

        domain.setSchedParams(params).should_be true
        params = domain.getSchedParams();
        //params.weight.should_be 30
        params.weight.should_be 50
    end

    it 'should return the domain security labels'
        try {
            var info = domain.getSecurityLabel();
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
        //info.label.should_not_be undefined
        //info.enforcing.should_not_be undefined
    end

    it 'should save a managed image of the domain'
        //test driver doesn't support these functions
        try {
            domain.saveManagedImage().should_be true
            domain.hasManagedImage().should_be true
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should remove a managed image of the domain'
        //test driver doesn't support these functions
        try {
            domain.removeManagedImage().should_be true
            domain.hasManagedImage().should_be false
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should allow to read the domain\'s memory content and return it in a Buffer object'
        var physical = [domain.VIR_MEMORY_PHYSICAL];
        var virtual = [domain.VIR_MEMORY_VIRTUAL];

        try {
            var psegment = domain.memoryPeek(0, 1024, physical);
            psegment.should_be_an_instance_of 'Buffer'

            var vsegment = domain.memoryPeek(0, 1024, virtual);
            vsegment.should_be_an_instance_of 'Buffer'
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should allow to read the content of a domain\'s disk device and return it in a Buffer object'
        try {
            var blocks = domain.blockPeek('/dev/sda', 0, 1024);
            blocks.should_be_an_instance_of 'Buffer'
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should return domain\'s memory statistics'
        //memory statistics in kbs
        try {
            var stats = domain.getMemoryStats();
            stats.swap_in.should_not_be undefined
            stats.swap_out.should_not_be undefined
            stats.major_fault.should_not_be undefined
            stats.minor_fault.should_not_be undefined
            stats.unused.should_not_be undefined
            stats.available.should_not_be undefined
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should return block device stats for block devices attached to the domain'
//        var stats = domain.getBlockStats('');
//        stats.read_requests.should_not_be undefined
//        stats.read_bytes.should_not_be undefined
//        stats.write_requests.should_not_be undefined
//        stats.write_bytes.should_not_be undefined
    end

    it 'should return basic information about a domain\'s block device'
        try {
            var info = domain.getBlockInfo('/path');
            info.capacity.should_not_be undefined
            info.allocation.should_not_be undefined
            info.physical.should_not_be undefined
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should dump the core of a domain on a given file for analysis'
        var path = '/tmp/dumpcore-test.txt';
        domain.coreDump(path).should_be true
    end

end

