require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

describe 'Libvirt'
  before_each 
    // local access, default config
    conn = new libvirt.Connection('test:///default')    

    // local access, custom config
    // conn = new libvirt.Connect('test:///path/to/driver/config.xml')
 
    // local access, default config, via daemon
    // conn = new libvirt.Connect('test+unix:///default')

    // remote access, TLS/x509
    // conn = new libvirt.Connect('test://example.com/default')
    
    // remote access, SASl/Kerberos
    // conn = new libvirt.Connect('test+tcp://example.com/default')
    
    // remote access, SSH tunnelled
    // conn = new libvirt.Connect('test+ssh://root@example.com/default')

  end
  
  after_each
    conn.close();
  end
        
  describe 'Connect'
    it 'should open a hypervisor connection'
        conn.should_not_be undefined
        conn.should_not_be null
    end
    
    it 'should open a hypervisor read-only connection'
        var readonly_conn = new libvirt.Connection('test:///default', true)
        readonly_conn.should_not_be undefined
        readonly_conn.should_not_be null
        readonly_conn.close();          
    end
  
    it 'should open an authenticated hypervisor connection'
    end
  
    it 'should compute the most feature-rich CPU'
        //TODO fixture with CPUs descriptions
        conn.getBaselineCPU()
    end
  
    it 'should close a hypervisor connection'
        conn.close().should_be true
    end
  
    it 'should return hypervisor capabilities'
        var capabilities = conn.getHypCapabilities()
        capabilities.should_not_be undefined
        capabilities.should_not_be null
    end
    
    it 'should return the hostname of the hypervisor host'
        var hostname = conn.getHypHostname()
        hostname.should_not_be undefined
        hostname.should_not_be null
    end
    
    it 'should return libvirt version used by remote running deamon'
        var version = conn.getRemoteLibVirtVersion()
        version.should_not_be undefined
        version.should_not_be null
    end
    
    it 'should return the maximum number of virtual CPUs supported for a guest VM'
        var max_vcpus = conn.getMaxVcpus()
        max_vcpus.should_not_be undefined
        max_vcpus.should_not_be null
    end
    
    it 'should return the name of the hypervisor software used'
        var hypervisor_type = conn.getHypervisorType()
        hypervisor_type.should_not_be undefined
        hypervisor_type.should_not_be null
    end
    
    it 'should return the URI of the hypervisor connection'
        var hypervisor_uri = conn.getHypervisorUri()
        hypervisor_uri.should_not_be undefined
        hypervisor_uri.should_not_be null
    end
    
    it 'should return version level of the running hypervisor'
        var hypervisor_version = conn.getHypervisorVersion()
        hypervisor_version.should_not_equal -1
    end

    it 'should not return version level of the running hypervisor if connection is read-only'
        var readonly_conn = new libvirt.Connection('test:///default', true)
        var hypervisor_version = readonly_conn.getHypervisorVersion()
        hypervisor_version.should_not_be undefined
        hypervisor_version.should_be null
    end 
    
    it 'should recognize if hypervisor connection is encrypted'
        var isEncrypted = conn.isEncrypted()
        isEncrypted.should_not_be undefined
        isEncrypted.should_not_be null
        isEncrypted.should_be false    
    end
    
    it 'should recognize if hypervisor connection is secure'
        var isSecure = conn.isSecure()
        isSecure.should_not_be undefined
        isSecure.should_not_be null
        //see http://libvirt.org/html/libvirt-libvirt.html#virConnectIsSecure
        isSecure.should_be true     
    end
    
    it 'should compare given cpu description with host CPU'
        //TODO cpu json fixture to compare with host cpu
        
    end
  end
end
