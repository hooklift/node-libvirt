require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;
StoragePool = libvirt.StoragePool;

//TODO DRY !!!
describe 'StoragePool'
    before
        hypervisor = new Hypervisor('test:///default');
        xml = fixture('storage_pool.xml');
    end

    before_each
        //pool = new StoragePool();
    end

    after_each
        //pool.stop();
    end

    after
        hypervisor.closeConnection();
    end

    it 'should create a transient pool'
        var pool = hypervisor.createStoragePool(xml);
        pool.getName().should_be 'virtimages'
    end

    it 'should define a pool'
        var pool = hypervisor.defineStoragePool(xml);
        pool.getName().should_be 'virtimages'
    end

    it 'should start an already defined pool'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        //workaround because the test driver seems start the pool when it's defined which is wrong
        //according to the documentation
        //see http://libvirt.org/html/libvirt-libvirt.html#virStoragePoolDefineXML
        if(pool.isActive()) {
            pool.stop();
        }
        pool.start().should_be true
        pool.isActive().should_be true
    end

    it 'should return volumes names'
        var pool = hypervisor.lookupStoragePoolByName('default-pool');
        var volumes = pool.getVolumes();
        volumes.should_be_an_instance_of Array
    end

    it 'should indicate if autostart is enabled'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.getAutostart().should_be true
    end

    it 'should set autostart to start the pool at boot time'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.setAutostart(false);
        pool.getAutostart().should_be false
    end

    it 'should return its information'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        var info = pool.getInfo();
        info.state.should_be pool.VIR_STORAGE_POOL_RUNNING
        info.capacity.should_be 107374182400
        info.allocation.should_be 0
        info.available.should_be 107374182400
    end

    it 'should be located by its name'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.getName().should_be 'virtimages'
    end

    it 'should be located by its uuid'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        var uuid = pool.getUUID();
        pool = hypervisor.lookupStoragePoolByUUID(uuid);
        pool.getName().should_be 'virtimages'
    end

    it 'should return its name'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.getName().should_be 'virtimages'
    end

    it 'should return its uuid'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        var uuid = pool.getUUID();
        pool = hypervisor.lookupStoragePoolByUUID(uuid);
        pool.getUUID().should_be uuid
    end

    it 'should return its xml representation'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.toXml([]).should_match '<name>virtimages</name>'
    end

    it 'should show if the pool is active'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.isActive().should_be true
    end

    it 'should show if the pool is persistent'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.isPersistent().should_be true
    end

    it 'should be refreshed'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.refresh().should_be true
    end

    it 'should stop an started pool'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.stop().should_be true
    end

    it 'should erase a pool'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.erase().should_be true
    end

    it 'should be undefined'
        var pool = hypervisor.lookupStoragePoolByName('virtimages');
        pool.undefine().should_be true
        try {
            hypervisor.lookupStoragePoolByName('virtimages');
        } catch(error) {
            error.message.should_match('Storage pool not found');
        }
    end
end

