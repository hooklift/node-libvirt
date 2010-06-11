require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

describe 'Domain'
    before
        var xml = fixture('domain.xml')
        hypervisor = new Hypervisor('test:///default')
        hypervisor.createDomain(xml);
    end

    before_each
        domain = hypervisor.lookupDomainByName('test_node_libvirt');
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
        var dom = hypervisor.lookupDomainByName('demo3');
        dom.getName().should_be 'demo3'
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

    it 'should show autostart is enable'
        domain.getAutostart().should_be true
    end

    it 'should set autostart'
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

    it 'should suspend and save the domain memory content to a file on disk'
        var name = domain.getName();
        domain.save('/tmp/'+name+'-saved.img').should_be true
    end

    it 'should restore a suspended and saved domain'
        var name = domain.getName();
        //test driver return exception because it really doesn't save a domain
        hypervisor.restoreDomain('/tmp/'+name+'-saved.img').should_be true
    end

    it 'should suspend a domain'
        domain.suspend().should_be true
    end

    it 'should resume a suspended domain'
        domain.resume().should_be true
    end

    it 'should shutdown a domain'
        domain.shutdown().should_be true
    end

    it 'should dynamically change the number of virtual CPUs used by the domain'
        domain.setVcpus(4).should_be true
        domain.getVcpus().should_be 4
    end

    /*it 'should create a non persistent Domain from its XML Description'
        var xml = fixture('domain.xml')
        var domain = hypervisor.createDomain(xml);
        domain.should_not_be undefined
        domain.should_not_be null
        sys.puts('domain: ' + sys.inspect(domain))
        sys.puts('object: ' + domain.toString());
    end*/
end

