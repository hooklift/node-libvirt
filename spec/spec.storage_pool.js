require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

describe 'StoragePool'
    before
        hypervisor = new Hypervisor('test:///default');
        xml = fixture('storage_pool.xml');
    end

    before_each
        pool = hypervisor.lookupStoragePoolByName('default-pool');
    end

    after
        hypervisor.closeConnection();
    end

    it 'should create a transient pool'
        var pool_ = hypervisor.createStoragePool(xml);
        pool_.getName().should_be 'virtimages'
    end

    it 'should define a pool'
        var pool_ = hypervisor.defineStoragePool(xml);
        pool_.getName().should_be 'virtimages'
    end

    it 'should start an already defined pool'
        //workaround because test driver seems to start the pool when it's defined which is wrong
        //according to the documentation
        //see http://libvirt.org/html/libvirt-libvirt.html#virStoragePoolDefineXML
        if(pool.isActive()) {
            pool.stop();
        }
        pool.start().should_be true
        pool.isActive().should_be true
    end

    it 'should return volumes names'
        var pool_ = hypervisor.lookupStoragePoolByName('default-pool');
        var volumes = pool_.getVolumes();
        volumes.should_be_an_instance_of Array
    end

    it 'should indicate if autostart is enabled'
        pool.getAutostart().should_be true
    end

    it 'should set autostart to start the pool at boot time'
        pool.setAutostart(false);
        pool.getAutostart().should_be false
    end

    it 'should return its information'
        var info = pool.getInfo();
        info.state.should_be pool.VIR_STORAGE_POOL_RUNNING
        info.capacity.should_be 107374182400
        info.allocation.should_be 0
        info.available.should_be 107374182400
    end

    it 'should be located by its name'
        pool.getName().should_be 'default-pool'
    end

    it 'should be located by its uuid'
        var uuid = pool.getUUID();
        var pool_ = hypervisor.lookupStoragePoolByUUID(uuid);
        pool_.getName().should_be 'default-pool'
    end

    it 'should return its name'
        pool.getName().should_be 'default-pool'
    end

    it 'should return its uuid'
        var uuid = pool.getUUID();
        pool_ = hypervisor.lookupStoragePoolByUUID(uuid);
        pool_.getUUID().should_be uuid
    end

    it 'should return its xml representation'
        pool.toXml([]).should_match '<name>default-pool</name>'
    end

    it 'should show if the pool is active'
        pool.isActive().should_be true
    end

    it 'should show if the pool is persistent'
        pool.isPersistent().should_be true
    end

    it 'should be refreshed'
        pool.refresh().should_be true
    end

    it 'should stop an started pool'
        pool.stop().should_be true
    end

    it 'should erase a pool'
        pool.erase().should_be true
    end

    it 'should be undefined'
        pool.undefine().should_be true
        try {
            hypervisor.lookupStoragePoolByName('default-pool');
        } catch(error) {
            error.message.should_match('Storage pool not found');
        }
    end
end

