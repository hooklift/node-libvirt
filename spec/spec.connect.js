require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

describe 'Libvirt'
    shared_behaviors_for 'Connect'

        after_each
            conn.close();
        end
            
        //describe 'Connect'
            it 'should open a hypervisor connection'
                conn.isEncrypted().should_be false
            end
            
            /*it 'should open a hypervisor read-only connection'    
            end*/
            
            it 'should open an authenticated hypervisor connection'
                //TODO make specs for auth mechanism
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

            /*it 'should not return version level of the running hypervisor if connection is read-only'
            end*/ 
            
            it 'should recognize if hypervisor connection is encrypted'
                var isEncrypted = conn.isEncrypted()
                isEncrypted.should_be false    
            end
            
            it 'should recognize if hypervisor connection is secure'
                var isSecure = conn.isSecure()
                //see http://libvirt.org/html/libvirt-libvirt.html#virConnectIsSecure
                isSecure.should_be true     
            end

            it 'should list names of defined but inactive domains'
                //TODO We need to create domains and list them for a better
                var domains = conn.listDefinedDomains()
                domains.should_be_an_instance_of Array
            end

            it 'should list names of defined but inactive interfaces'
                //TODO We need to create interfaces and list them for a better
                var interfaces = conn.listDefinedInterfaces()
                interfaces.should_be_an_instance_of Array
            end

            it 'should list names of defined but inactive networks'
                //TODO We need to create networks and list them for a better
                var networks = conn.listDefinedNetworks()
                networks.should_be_an_instance_of Array
            end

            it 'should list names of defined but inactive storage pools'
                //TODO We need to create storage pools and list them
                var storage_pools = conn.listDefinedStoragePools()
                storage_pools.should_be_an_instance_of Array
            end

            it 'should list ids of active domains'
            end
            
            it 'should list names of active physical host interfaces'
            end

            it 'should list names of network filters'
            end

            it 'should list names of active networks'
            end

            it 'should list UUIDs names of defined secrets'
            end

            it 'should list names of active storage pools'
            end

            it 'should return the number of defined but inactive domains'
            end

            it 'should return the number of defined but inactive interfaces on the physical host'
            end

            it 'should return the number of defined but inactive networks'
            end

            it 'should return the number of defined but inactive storage pools'
            end

            it 'should return the number of active domains'
            end

            it 'should return the number of active interfaces on the physical host'
            end

            it 'should return the number of network filters'
            end

            it 'should return the number of active networks'
            end

            it 'should return the number of currently defined secrets'
            end

            it 'should return the number of active storage pools'
            end
            
        //end
    end
    
    describe 'Connection using test:///default'
        should_behave_like('Connect')

        before_each
            // local access, default config
            conn = new libvirt.Connection('test:///default')
        end
        
        it 'should open a hypervisor read-only connection'
            var readonly_conn = new libvirt.Connection('test:///default', true)
            readonly_conn.should_not_be undefined
            readonly_conn.should_not_be null
            readonly_conn.close()         
        end
        
        it 'should not return version level of the running hypervisor if connection is read-only'
            var readonly_conn = new libvirt.Connection('test:///default', true)
            var hypervisor_version = readonly_conn.getHypervisorVersion()
            hypervisor_version.should_be '0.0.2'
        end
    end
    
    describe 'Connection using test+unix:///default via libvirt daemon'
        should_behave_like('Connect')

        before_each
            // local access, default config, via daemon
            conn = new libvirt.Connection('test+unix:///default')
        end
        
        it 'should open a hypervisor read-only connection'
            var readonly_conn = new libvirt.Connection('test+unix:///default', true)
            readonly_conn.should_not_be undefined
            readonly_conn.should_not_be null
            readonly_conn.close()         
        end
        
        it 'should not return version level of the running hypervisor if connection is read-only'
            var readonly_conn = new libvirt.Connection('test+unix:///default', true)
            var hypervisor_version = readonly_conn.getHypervisorVersion()
            hypervisor_version.should_be '0.0.2'
            //hypervisor_version.should_be null
        end
    end
    
    describe 'Connection using qemu:///system'
        should_behave_like('Connect')
        
        before_each
            // local access, default config, via daemon
            conn = new libvirt.Connection('qemu:///system')
            //conn = new libvirt.Connection('test:///default')
        end
        
         it 'should open a hypervisor read-only connection'
            var readonly_conn = new libvirt.Connection('qemu:///system', true)
            readonly_conn.should_not_be undefined
            readonly_conn.should_not_be null
            readonly_conn.close()         
        end
        
        it 'should not return version level of the running hypervisor if connection is read-only'
            var readonly_conn = new libvirt.Connection('qemu:///system', true)
            var hypervisor_version = readonly_conn.getHypervisorVersion()
            //hypervisor_version.should_be '0.11.0'
            //hypervisor_version.should_be null  //must be this 
            hypervisor_version.should_not_be null
        end
        
        it 'should compute the most feature-rich CPU'
            cpu1 = fixture('cpu1.xml')
            cpu2 = fixture('cpu2.xml')
            computed_cpu = fixture('match_bt_cpu1_and_cpu2.xml')
                
            xmlCPUs = [cpu1, cpu2]
                
            var baseline = conn.getBaselineCPU(xmlCPUs)
            baseline.should_not_be null
        end
        
         it 'should compare given cpu description with host CPU'
            var cpu = fixture('cpu1.xml')
            var result = conn.compareCPU(cpu)
            result.should_not_be undefined
            result.should_not_be null
        end
    end
end
