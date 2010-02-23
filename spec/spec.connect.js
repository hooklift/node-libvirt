describe 'Connect'
  before_each
 
    // local access, default config
    conn = new libvirt.Connect('test:///default') 

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
        
  describe 'Core'
    it 'should open a hypervisor connection'
    end
    
    it 'should open a hypervisor read-only connection'
    end
  
    it 'should open an authenticated hypervisor connection'
    end
  
    it 'should compute the most feature-rich CPU'
    end
  
    it 'should close hypervisor connection'
    end
  
    it 'should return hypervisor capabilities'
    end
    
    it 'should return the hostname of the hypervisor host'
    end
    
    it 'should return libvirt version used by remote running deamon'
    end
    
    it 'should return maximum number of virtual CPUs supported for a guest VM'
    
    end
    
    it 'should return the name of the hypervisor software used'
    end
    
    it 'should return the URI of the hypervisor connection'
    end
    
    it 'should return version level of the running hypervisor'
    end
    
    it 'should not return version level of the running hypervisor if connection is read-only'
    end 
    
    it 'should recognize if hypervisor connection is encrypted'
    end
    
    it 'should recognize if hypervisor connection is secure'
    end
    
    it 'should compare given cpu description with host CPU'
    end
  end

  describe 'Manage Domains'    
    it 'should allow register a callback function for domain events'
    end
    
    it 'should allow deregister a callback function for domain events'
    end
    
    it 'should invoke registered callback function when a domain event occurs'
    end
  end

  describe 'Manage Networks'

  end

  describe 'Manage Intefaces'
   
  end
  
  describe 'Manage Node Devices'
  
  end
  
  describe 'Manage Storage Pools'
  
  end
  
  describe 'Manage Storage Volumes'
  
  end
  
  describe 'Event Layer'
  
  end
  
end
