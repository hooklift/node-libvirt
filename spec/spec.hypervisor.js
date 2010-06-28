require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

//TODO improve using libxmljs and xpath
describe 'Hypervisor'
    before_each
        hypervisor = new Hypervisor('test:///default')
    end
    after_each
        hypervisor.closeConnection();
    end

    it 'should open a hypervisor connection'
        hypervisor.getVersion().should_not_be null
    end

    it 'should open a hypervisor read-only connection'
        var readonly_conn = new Hypervisor('test:///default', true)
        readonly_conn.should_not_be undefined
        readonly_conn.should_not_be null
        readonly_conn.closeConnection()
    end

    it 'should not return version level of the running hypervisor if connection is read-only'
        var readonly_conn = new Hypervisor('test:///default', true)
        var version = readonly_conn.getVersion()
        version.should_be '0.0.2'
    end

    it 'should open an authenticated hypervisor connection'
//                var authdata = { credlist: [ hypervisor.VIR_CRED_AUTHNAME,
//                                             hypervisor.VIR_CRED_PASSPHRASE],
//
//                                 callback: function(credentials) {
//                                    for(cred in credentials) {
//                                        if(cred.type == hypervisor.VIR_CRED_AUTHNAME) {
//                                            cred.result = 'camilo'
//                                        }
//                                        if(cred.type == hypervisor.VIR_CRED_PASSPHRASE) {
//                                            cred.result = 'password'
//                                        }
//                                    }
//                                }
//
//                        };
//
//                var hypervisor = new libvirt.Hypervisor('qemu:///system', authdata);
//                hypervisor.should_not_be undefined
//                hypervisor.should_not_be null
//                hypervisor.getVersion().should_not_be null
    end

    it 'should close a hypervisor connection'
        hypervisor.closeConnection().should_be true
    end

    it 'should return hypervisor capabilities'
        var capabilities = hypervisor.getCapabilities()
        capabilities.should_not_be null
    end

    it 'should return the hostname of the hypervisor host'
        var hostname = hypervisor.getHostname()
        hostname.should_not_be null
    end

    it 'should return libvirt version used by remote running deamon'
        var version = hypervisor.getLibVirtVersion()
        version.should_not_be null
    end

    it 'should return the maximum number of virtual CPUs supported for a guest VM'
        var max_vcpus = hypervisor.getMaxVcpus()
        max_vcpus.should_not_be null
    end

    it 'should return the name of the hypervisor software used'
        var type = hypervisor.getType()
        type.should_not_be null
    end

    it 'should return the URI of the hypervisor connection'
        var uri = hypervisor.getConnectionUri()
        uri.should_not_be null
    end

    it 'should return version level of the running hypervisor'
        var version = hypervisor.getVersion()
        version.should_not_equal null
    end

    /*it 'should not return version level of the running hypervisor if connection is read-only'
    end*/

    it 'should recognize if hypervisor connection is encrypted'
        var isEncrypted = hypervisor.isConnectionEncrypted()
        isEncrypted.should_be false
    end

    it 'should recognize if hypervisor connection is secure'
        var isSecure = hypervisor.isConnectionSecure()
        //see http://libvirt.org/html/libvirt-libvirt.html#virConnectIsSecure
        isSecure.should_be true
    end

    it 'should list names of defined but inactive domains'
        //TODO We need to create domains and list them for a better spec
        var domains = hypervisor.getDefinedDomains()
        domains.should_be_an_instance_of Array
    end

    /*it 'should list names of defined but inactive interfaces'
        //TODO We need to create interfaces and list them for a better
        var interfaces = hypervisor.getDefinedInterfaces()
        interfaces.should_be_an_instance_of Array
    end*/

    it 'should list names of defined but inactive networks'
        //TODO We need to create networks and list them for a better
        var networks = hypervisor.getDefinedNetworks()
        networks.should_be_an_instance_of Array
    end

    it 'should list names of defined but inactive storage pools'
        //TODO We need to create storage pools and list them
        var storage_pools = hypervisor.getDefinedStoragePools()
        storage_pools.should_be_an_instance_of Array
    end

    it 'should list ids of active domains'
        var ids = hypervisor.getActiveDomains();
        ids.should_be_an_instance_of Array
    end

    it 'should list names of active physical host interfaces'
        var interfaces = hypervisor.getActiveInterfaces();
        interfaces.should_be_an_instance_of Array
    end

    it 'should list names of active networks'
        var networks = hypervisor.getActiveNetworks();
        networks.should_be_an_instance_of Array
        /*for(network in networks) {
            sys.puts('======');
            sys.puts(networks[network]);
        }*/
    end

    it 'should list UUIDs names of defined secrets'
        //no supported by test driver
        try {
            var uuids = hypervisor.getSecrets();
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should list names of active storage pools'
        var names = hypervisor.getActiveStoragePools();
        names.should_be_an_instance_of Array
    end

    it 'should return the number of defined but inactive domains'
        var num = hypervisor.getNumberOfDefinedDomains();
        num.should_be 0
    end

    it 'should return the number of defined but inactive interfaces on the physical host'
        var num = hypervisor.getNumberOfDefinedInterfaces();
        num.should_be 0
    end

    it 'should return the number of defined but inactive networks'
        var num = hypervisor.getNumberOfDefinedNetworks();
        num.should_be 0
    end

    it 'should return the number of defined but inactive storage pools'
        var num = hypervisor.getNumberOfDefinedStoragePools();
        num.should_be 0
    end

    it 'should return the number of active domains'
        var num = hypervisor.getNumberOfActiveDomains();
        num.should_be 1
    end

    it 'should return the number of active interfaces on the physical host'
        var num = hypervisor.getNumberOfActiveInterfaces();
        num.should_be 1
    end

    it 'should return the number of active networks'
        var num = hypervisor.getNumberOfActiveNetworks();
        num.should_be 1
    end

    it 'should return the number of currently defined secrets'
        //no supported by test driver
        try {
            var num = hypervisor.getNumberOfSecrets();
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should return the number of active storage pools'
        var num = hypervisor.getNumberOfActiveStoragePools();
        num.should_be 1
    end

    it 'should return the number of network filters'
        //no supported by test driver
        try {
            var num = hypervisor.getNumberOfNetworkFilters();
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should list names of network filters'
        //no supported by test driver
        try {
            var filters = hypervisor.getNetworkFilters();
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should have the constant VIR_CRED_USERNAME'
        hypervisor.VIR_CRED_USERNAME.should_not_be undefined
        hypervisor.VIR_CRED_USERNAME.should_be 1
    end

    it 'should have the constant VIR_CRED_AUTHNAME'
        hypervisor.VIR_CRED_AUTHNAME.should_not_be undefined
        hypervisor.VIR_CRED_AUTHNAME.should_be 2
    end

    it 'should have the constant VIR_CRED_LANGUAGE'
        hypervisor.VIR_CRED_LANGUAGE.should_not_be undefined
        hypervisor.VIR_CRED_LANGUAGE.should_be 3
    end

    it 'should have the constant VIR_CRED_CNONCE'
        hypervisor.VIR_CRED_CNONCE.should_not_be undefined
        hypervisor.VIR_CRED_CNONCE.should_be 4
    end

    it 'should have the constant VIR_CRED_PASSPHRASE'
        hypervisor.VIR_CRED_PASSPHRASE.should_not_be undefined
        hypervisor.VIR_CRED_PASSPHRASE.should_be 5
    end

    it 'should have the constant VIR_CRED_ECHOPROMPT'
        hypervisor.VIR_CRED_ECHOPROMPT.should_not_be undefined
        hypervisor.VIR_CRED_ECHOPROMPT.should_be 6
    end

    it 'should have the constant VIR_CRED_NOECHOPROMPT'
        hypervisor.VIR_CRED_NOECHOPROMPT.should_not_be undefined
        hypervisor.VIR_CRED_NOECHOPROMPT.should_be 7
    end

    it 'should have the constant VIR_CRED_REALM'
        hypervisor.VIR_CRED_REALM.should_not_be undefined
        hypervisor.VIR_CRED_REALM.should_be 8
    end

    it 'should have the constant VIR_CRED_EXTERNAL'
        hypervisor.VIR_CRED_EXTERNAL.should_not_be undefined
        hypervisor.VIR_CRED_EXTERNAL.should_be 9
    end

    it 'should have the constant VIR_CPU_COMPARE_ERROR'
        hypervisor.VIR_CPU_COMPARE_ERROR.should_not_be undefined
        hypervisor.VIR_CPU_COMPARE_ERROR.should_be -1
    end

    it 'should have the constant VIR_CPU_COMPARE_INCOMPATIBLE'
        hypervisor.VIR_CPU_COMPARE_INCOMPATIBLE.should_not_be undefined
        hypervisor.VIR_CPU_COMPARE_INCOMPATIBLE.should_be 0
    end

    it 'should have the constant VIR_CPU_COMPARE_IDENTICAL'
        hypervisor.VIR_CPU_COMPARE_IDENTICAL.should_not_be undefined
        hypervisor.VIR_CPU_COMPARE_IDENTICAL.should_be 1
    end

    it 'should have the constant VIR_CPU_COMPARE_SUPERSET'
        hypervisor.VIR_CPU_COMPARE_SUPERSET.should_not_be undefined
        hypervisor.VIR_CPU_COMPARE_SUPERSET.should_be 2
    end

    it 'should compute the most feature-rich CPU'
        cpu1 = fixture('cpu1.xml')
        cpu2 = fixture('cpu2.xml')
        computed_cpu = fixture('match_bt_cpu1_and_cpu2.xml')

        xmlCPUs = [cpu1, cpu2]

        //no supported by test driver
        try {
            hypervisor.getBaselineCPU(xmlCPUs);
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should compare given cpu description with host CPU'
        var cpu = fixture('cpu1.xml')
        //no supported by test driver
        try {
          hypervisor.compareCPU(cpu)
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should return the amount of node free memory in one or more NUMA cells'
        var startCell = 0;
        var maxCells = 2;
        var memory = hypervisor.getNodeFreeMemoryInNumaCells(startCell, maxCells);
        memory.should_be_an_instance_of Array

        memory[0].should_be 2097152
        memory[1].should_be 4194304
    end

    it 'should return the node information where hypervisor is running'
        var info = hypervisor.getNodeInfo();
        info.model.should_not_be null
        info.memory.should_not_be null
        info.cpus.should_not_be null
        info.mhz.should_not_be null
        info.nodes.should_not_be null
        info.sockets.should_not_be null
        info.cores.should_not_be null
        info.threads.should_not_be null
    end

    it 'should return free memory of the physical node'
        try {
            var memory = hypervisor.getNodeFreeMemory();
            memory.should_not_be null
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should list node devices names'
        var devices = hypervisor.getNodeDevicesNames();
        devices.should_be_an_instance_of Array
    end

    it 'should return the node security model'
        try {
            var security = hypervisor.getNodeSecurityModel();
            security.model.should_not_be undefined
            security.doi.should_not_be undefined
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end
end

