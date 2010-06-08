require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

describe 'Domain'
    before_each
        hypervisor = new Hypervisor('test:///default')
    end

    after_each
        hypervisor.closeConnection();
    end

    it 'should lookup a domain by id'
        var domain = hypervisor.lookupById(1);
        domain.should_not_be null
    end

    it 'should create a persistent Domain from its XML Description'
        var xml = fixture('domain.xml')
        var domain = hypervisor.createDomain(xml, true);
        domain.should_not_be undefined
        domain.should_not_be null
        //sys.puts('domain: ' + sys.inspect(domain))
        //sys.puts('domain: ' + domain.toString());
        //domain.destroy()
        //domain.should_be undefined
    end

    it 'should create a non persistent Domain from its XML Description'
        var xml = fixture('domain.xml')
        var domain = hypervisor.createDomain(xml);
        domain.should_not_be undefined
        domain.should_not_be null
        domain.os_type.should_be 'linux'
        sys.puts('domain: ' + sys.inspect(domain))
        sys.puts('object: ' + domain.toString());
    end
end

