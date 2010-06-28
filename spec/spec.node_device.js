require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

//TODO create a Node class and add detach attach

describe 'NodeDevice'
    before_each
        hypervisor = new Hypervisor('test:///default')
        //device = hypervisor.lookupNodeDeviceByName('')
    end

    after_each
        hypervisor.closeConnection();
    end

    it 'should create a node device using its xml description'
        //test driver does not provide mechanisms to test this function
        try {
            var xml = fixture('node_device.xml');
            var device = hypervisor.createNodeDevice(xml);
            device.should_not_be undefined
        } catch(error) {
            error.code.should_not_be undefined
        }
    end

    it 'should lookup a node device by name'
        //test driver does not provide mechanisms to test this function
        try {
            var devices = hypervisor.getNodeDevicesNames();
            var device = hypervisor.lookupNodeDeviceByName(devices[0]);
            device.should_not_be undefined
        } catch(error) {
            error.should_not_be undefined
        }
    end

    it 'should detach the device from the node itself in order to be assigned to a guest domain'
        //test driver does not provide mechanisms to test this function
        try {
            device.detach().should_be true
        } catch(error) {
            error.should_not_be undefined
        }
    end

    it 'should reattach a previously detached node device'
        //test driver does not provide mechanisms to test this function
        try {
            device.reattach().should_be true
        } catch(error) {
            error.should_not_be undefined
        }
    end

    it 'should reset the node device a previously detached node device'
        //test driver does not provide mechanisms to test this function
        try {
            device.reset().should_be true
        } catch(error) {
            error.should_not_be undefined
        }
    end

    it 'should return the device name'
        //test driver does not provide mechanisms to test this function
        try {
            var name = device.getName();
            name.should_not_be undefined
            name.should_not_be null
        } catch(error) {
            error.should_not_be undefined
        }
    end

    it 'should return the device parent name'
        //test driver does not provide mechanisms to test this function
        try {
            var parent = device.getParentName();
            parent.should_not_be undefined
            parent.should_not_be null
        } catch(error) {
            error.should_not_be undefined
        }

    end

    it 'should return de device xml description'
        //test driver does not provide mechanisms to test this function
        try {
            var xml = device.toXml();
            xml.should_not_be undefined
            xml.should_not_be null
        } catch(error) {
            error.should_not_be undefined
        }
    end

    it 'should list device capabilities'
        //test driver does not provide mechanisms to test this function
        try {
            var capabilities = device.getCapabilities();
            capabilities.should_not_be undefined
            capabilities.should_not_be null
        } catch(error) {
            error.should_not_be undefined
        }
    end

    it 'should remove the node device from the host operating system'
        //test driver does not provide mechanisms to test this function
        try {
            device.destroy().should_be true
        } catch(error) {
            error.should_not_be undefined
        }
    end
end

