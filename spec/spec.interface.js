require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

describe 'Interface'
    before
        hypervisor = new Hypervisor('test:///default');
    end

    before_each
        interface = hypervisor.lookupInterfaceByName('eth1');
    end

    after
        hypervisor.closeConnection();
    end

    it 'should define the interface from its xml description'
        var xml = fixture('interface.xml');
        var iface = hypervisor.defineInterface(xml);
        iface.getName().should_be 'eth0'
    end

    it 'should undefine the interface'
        var iface = hypervisor.lookupInterfaceByName('eth0');
        iface.undefine().should_be true
    end

    it 'should be located through its name'
         var iface = hypervisor.lookupInterfaceByName('eth1');
         iface.getName().should_be 'eth1'
    end

    it 'should be located through its mac address'
        var iface = hypervisor.lookupInterfaceByMacAddress('aa:bb:cc:dd:ee:ff');
        iface.getName().should_be 'eth1'
    end

    it 'should stop'
        interface.stop().should_be true
    end

    it 'should start'
        interface.start().should_be true
    end

    it 'should return the name'
        interface.getName().should_be 'eth1'
    end

    it 'should return the mac address'
        interface.getMacAddress().should_be 'aa:bb:cc:dd:ee:ff'
    end

    it 'should return its xml description'
        interface.toXml([]).should_match 'eth1'
    end

end

