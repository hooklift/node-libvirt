require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

describe 'Network'
    before
        hypervisor = new Hypervisor('test:///default');
    end

    before_each
        network = hypervisor.lookupNetworkByName('default');
    end

    after_each
        network.destroy();
    end

    after
        hypervisor.closeConnection();
    end

    it 'should create and start an already defined network'
        var xml = fixture('network.xml');
        var net = hypervisor.defineNetwork(xml);
        net.start().should_be true
    end

    it 'should create and start a new virtual network from its xml description'
        var xml = fixture('network.xml');
        var net = hypervisor.createNetwork(xml);
        net.getName().should_be 'test'
    end

    it 'should lookup the network by name'
        var net = hypervisor.lookupNetworkByName('default');
        net.getName().should_be 'default'
    end

    it 'should lookup the network by uuid'
        var uuid = network.getUUID();
        var net = hypervisor.lookupNetworkByUUID(uuid);
        net.getName().should_be 'default'
    end

    it 'should determinate if the network has a persistent configuration'
        network.isPersistent().should_be true
    end

    it 'should determinate if the network is currently running'
        network.isActive().should_be false
        network.start();
        network.isActive().should_be true
    end

    it 'should provide a xml network description'
        var xml = network.toXml([]);
        xml.should_match('<name>default</name>');
    end

    it 'should return the network uuid'
        network.getUUID().should_not_be undefined
        network.getUUID().should_not_be null
    end

    it 'should return the network name'
        network.getName().should_be 'default'
    end

    it 'should return the bridge interface name'
        var name = network.getBridgeName();
        name.should_be 'virbr0'
    end

    it 'should indicate if the network is configured to be automatically started when the host boots'
        network.getAutostart().should_be true
    end

    it 'should configure the network to be automatically started when the host boots'
        network.setAutostart(false).should_be true
        network.getAutostart().should_be false
        network.setAutostart(true).should_be true
        network.getAutostart().should_be true
    end

    it 'should define a network from its xml description'
        var xml = fixture('network.xml');
        var net = hypervisor.defineNetwork(xml);
        net.getName().should_be 'test'
    end

    it 'should undefine a network'
        var net = hypervisor.lookupNetworkByName('test');
        net.destroy().should_be true
        net.undefine().should_be true
    end

    it 'should destroy a network'
        network.destroy().should_be true
    end

end

