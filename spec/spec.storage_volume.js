require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

describe 'StorageVolume'
    before
        var xml  = fixture('storage_volume.xml');
        hypervisor = new Hypervisor('test:///default');
        pool = hypervisor.lookupStoragePoolByName('default-pool');
        volume = pool.createVolume(xml);
    end

    after
        //jspec async bug. This statement is being executed before the end of "it should be deleted" spec.
        //volume.remove();
        hypervisor.closeConnection();
    end

    it 'should be created'
        volume.getName().should_be 'sparse.img'
    end

    it 'should return volume information'
        var info = volume.getInfo();
        info.type.should_be volume.VIR_STORAGE_VOL_FILE
        //bytes
        info.capacity.should_be 5368709120
        info.allocation.should_be 0
    end

    it 'should be wiped'
        try {
            volume.wipe().should_be true
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should return its key'
        volume.getKey().should_be '/default-pool/sparse.img'
    end

    it 'should return its name'
        volume.getName().should_be 'sparse.img'
    end

    it 'should return its path'
        volume.getPath().should_be '/default-pool/sparse.img'
    end

    it 'should return its xml description'
        volume.toXml().should_match '<name>sparse.img</name>'
    end

    it 'should be located by its key'
        var volume_  = hypervisor.lookupStorageVolumeByKey(volume.getKey());
        volume_.getName().should_be volume.getName()
    end

    it 'should be located by its name'
        var volume_ = pool.lookupVolumeByName(volume.getName());
        volume_.getKey().should_be volume.getKey()
    end

    it 'should be located by its path'
        var volume_ = hypervisor.lookupStorageVolumeByPath(volume.getPath());
        volume_.getKey().should_be volume.getKey()
    end

    it 'should be removed from the pool'
        volume.remove().should_be true
    end
end

