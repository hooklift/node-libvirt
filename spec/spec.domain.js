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
        var vcpus = domain.getVcpus(); //return an array
        vcpus.should_be_an_instance_of Array
        vcpus[0].number.should_not_be undefined
        vcpus[0].state.should_not_be undefined
        vcpus[0].cpu_time.should_not_be undefined
        vcpus[0].cpu.should_not_be undefined
        vcpus[0].affinity.should_be_an_instance_of Array

        var affinity = vcpus[0].affinity;
        affinity[0].usable.should_not_be undefined
    end

    it 'should attach a device'
        var device = fixture('device.xml');
        //no supported by test driver
        try {
            domain.attachDevice(device);
        } catch(error){
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

end

