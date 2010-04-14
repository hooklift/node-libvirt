require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

describe 'Libvirt'
    before_each 
        // local access, default config
        conn = new libvirt.Connection('test:///default')

        // local access, custom config
        // conn = new libvirt.Connection('test:///path/to/driver/config.xml')
        
        // local access, default config, via daemon
        //conn = new libvirt.Connection('test+unix:///default')

        // remote access, TLS/x509
        // conn = new libvirt.Connection('test://example.com/default')
        
        // remote access, SASl/Kerberos
        // conn = new libvirt.Connection('test+tcp://example.com/default')
        
        // remote access, SSH tunnelled
        //conn = new libvirt.Connection('test+ssh://camilo@example.com/default')
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
            readonly_conn.close()         
        end
        
        it 'should open an authenticated hypervisor connection'
        end
        
        it 'should compute the most feature-rich CPU'
            cpu1 = fixture('cpu1.xml')
            cpu2 = fixture('cpu2.xml')
            xmlCPUs = [cpu1, cpu2]
            
            var baseline = conn.getBaselineCPU(xmlCPUs)
            baseline.should_be null
            baseline.should_not_be undefined
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
            
            if(conn.getHypervisorType() == 'Test'){
                hypervisor_version.should_be 2
            } else {
                hypervisor_version.should_be null
            }   
        end 
        
        it 'should recognize if hypervisor connection is encrypted'
            var isEncrypted = conn.isEncrypted()
            isEncrypted.should_be false    
        end
        
        it 'should recognize if hypervisor connection is secure'
            var isSecure = conn.isSecure()
            //see http://libvirt.org/html/libvirt-libvirt.html#virConnectIsSecure
            isSecure.should_be true     
        end
        
        it 'should compare given cpu description with host CPU'
           /*describe 'when is incompatible'
                var cpu = fixture('cpu1.xml')
                var result = conn.compareCPU(cpu)
                result.should_be 0 //VIR_CPU_COMPARE_INCOMPATIBLE  
            end*/
            
            /*describe 'when is identical'
                var cpu = fixture('cpu3.xml')
            end 
            
            describe 'when is a superset'
                var cpu = fixture('cpu4.xml')
            end
            
            describe 'there is an error'
                var cpu = fixture('cpu5.xml')
                var result = conn.compareCPU(cpu)
                result.should_be -1
            end */
        end
    end
end
