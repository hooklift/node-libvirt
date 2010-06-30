require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

describe 'NetworkFilter'
    before
        hypervisor = new Hypervisor('test:///default');
    end

    before_each
        //filter = hypervisor.defineNetworkFilter(xml);
    end

    after
        hypervisor.closeConnection();
    end

    it 'should define network filter from its xml description'
        //test driver does not provide mechanisms to test this function
        try {
            var xml = fixture('network_filter.xml');
            hypervisor.defineNetworkFilter(xml).should_be true
        } catch(error) {
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should return the network filter name'
        //test driver does not provide mechanisms to test this function
        //filter.getName().should_be 'default'
    end

    it 'should return the network filter UUID'
        //test driver does not provide mechanisms to test this function
        //filter.GetUUID().should_not_be undefined
    end

    it 'should return the xml description of the network filter'
        //test driver does not provide mechanisms to test this function
        //var xml = filter.toXml();
    end

    it 'should look up the network filter based in its name'
        //test driver does not provide mechanisms to test this function
        try {
            var filter = hypervisor.lookupNetworkFilterByName('test-eth0');
            filter.getName().should_be 'default'
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should look up the network filter based in its UUID'
        //test driver does not provide mechanisms to test this function
        try {
            var filter = hypervisor.lookupNetworkFilterByName('test-eth0');
            var filter2 = hypervisor.lookupNetworkFilterByUUID(filter.getUUID());
            filter2.getName().should_be 'test-eth0'
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should undefine the network filter'
        //test driver does not provide mechanisms to test this function
        //filter.undefine().should_be true
    end
end

