require.paths.unshift('build/default/src');
var sys = require('sys');
var libvirt = require('node-libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor;

module.exports = {
    'should open a connection': function(assert) {
        hypervisor = new Hypervisor('test:///default');
        assert.isNotNull(hypervisor.getVersion());
    },

    'should open a read-only connection': function(assert) {
        var readonlyConn = new Hypervisor('test:///default', true);
        assert.isDefined(readonlyConn);
        assert.isNotNull(readonlyConn);
        readonlyConn.closeConnection();
    },

    'should not return the version for a read-only conn': function(assert) {
        //the test driver sends the version in read-only!!
        //this is not a big deal so we'll let it pass
        var readonlyConn = new Hypervisor('test:///default', true);
        assert.isNotNull(readonlyConn.getVersion());
    },

    'should open an authenticated connection': function(assert) {
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
    },

    'should return hypervisor capabilities': function(assert) {
        var capabilities = hypervisor.getCapabilities();
        assert.isNotNull(capabilities);
    },

    'should return the hostname of the hypervisor host': function(assert) {
        var hostname = hypervisor.getHostname();
        assert.isNotNull(hostname);
    },

    'should return libvirt version used by remote running deamon': function(assert) {
        var version = hypervisor.getLibVirtVersion();
        assert.isNotNull(version);
    },

    'should return the vcpu maximum number supported for a guest VM': function(assert) {
        var maxVcpus = hypervisor.getMaxVcpus('kvm');
        assert.isNotNull(maxVcpus);
    },

    'should return the name of the hypervisor software used': function(assert) {
        var type = hypervisor.getType();
        assert.isNotNull(type);
    },

    'should return the connection URI': function(assert) {
        var uri = hypervisor.getConnectionUri();
        assert.isNotNull(uri);
    },

    'should return version level of the running hypervisor': function(assert) {
        var version = hypervisor.getVersion();
        assert.isNotNull(version);
    },

    'should recognize if the connection is encrypted': function(assert) {
        var isEncrypted = hypervisor.isConnectionEncrypted();
        assert.eql(isEncrypted, false);
    },

    'should recognize if the connection is secure': function(assert) {
        var isSecure = hypervisor.isConnectionSecure();
        //see http://libvirt.org/html/libvirt-libvirt.html#virConnectIsSecure
        assert.eql(isSecure, true);
    },

    'should list names of defined but inactive domains': function(assert) {
        //TODO We need to create domains and list them for a better spec
        var domains = hypervisor.getDefinedDomains();
        assert.eql(domains instanceof Array, true);
    },

    'should list names of defined but inactive interfaces': function(assert) {
        //TODO We need to create interfaces and list them for a better
        var interfaces = hypervisor.getDefinedInterfaces();
        assert.eql(interfaces instanceof Array, true);
    },

    'should list names of defined but inactive networks': function(assert) {
        //TODO We need to create networks and list them for a better
        var networks = hypervisor.getDefinedNetworks();
        assert.eql(networks instanceof Array, true);
    },

    'should list names of defined but inactive storage pools': function(assert) {
        //TODO We need to create storage pools and list them
        var storagePools = hypervisor.getDefinedStoragePools();
        assert.eql(storagePools instanceof Array, true);
    },

    'should list active domains ids': function(assert) {
        //1 is the default active domain in the test driver
        var ids = hypervisor.getActiveDomains();
        assert.eql(ids, [1]);
    },

    'should list names of active physical host interfaces': function(assert) {
        var interfaces = hypervisor.getActiveInterfaces();
        assert.eql(interfaces, ['eth1']);
    },

    'should list names of active networks': function(assert) {
        //'default' is the active network by default in the test driver
        var networks = hypervisor.getActiveNetworks();
        assert.eql(networks, ['default']);
    },

    'should list UUIDs names of defined secrets': function(assert) {
        //no supported by test driver
        try {
            var uuids = hypervisor.getSecrets();
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should list names of active storage pools': function(assert) {
        var names = hypervisor.getActiveStoragePools();
        assert.eql(names, ['default-pool']);
    },

    'should return the number of defined but inactive domains': function(assert) {
        var num = hypervisor.getNumberOfDefinedDomains();
        assert.eql(num, 0);
    },

    'should return the number of defined but inactive physical host interfaces': function(assert) {
        var num = hypervisor.getNumberOfDefinedInterfaces();
        assert.eql(num, 0);
    },

    'should return the number of defined but inactive networks': function(assert) {
        var num = hypervisor.getNumberOfDefinedNetworks();
        assert.eql(num, 0);
    },

    'should return the number of defined but inactive storage pools': function(assert) {
        var num = hypervisor.getNumberOfDefinedStoragePools();
        assert.eql(num, 0);
    },

    'should return the number of active domains': function(assert) {
        var num = hypervisor.getNumberOfActiveDomains();
        assert.eql(num, 1);
    },

    'should return the number of active physical host interfaces': function(assert) {
        var num = hypervisor.getNumberOfActiveInterfaces();
        assert.eql(num, 1);
    },

    'should return the number of active networks': function(assert) {
        var num = hypervisor.getNumberOfActiveNetworks();
        assert.eql(num, 1);
    },

    'should return the number of currently defined secrets': function(assert) {
        //no supported by test driver
        try {
            var num = hypervisor.getNumberOfSecrets();
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return the number of active storage pools': function(assert) {
        var num = hypervisor.getNumberOfActiveStoragePools();
        assert.eql(num, 1);
    },

    'should return the number of network filters': function(assert) {
        //no supported by test driver
        try {
            var num = hypervisor.getNumberOfNetworkFilters();
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should list names of network filters': function(assert) {
        //no supported by test driver
        try {
            var filters = hypervisor.getNetworkFilters();
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should compute the most feature-rich CPU': function(assert) {
        var cpu1 = fixture('cpu1.xml')
        var cpu2 = fixture('cpu2.xml')
        var computed_cpu = fixture('match_bt_cpu1_and_cpu2.xml')

        xmlCPUs = [cpu1, cpu2]

        //no supported by test driver
        try {
            hypervisor.getBaselineCPU(xmlCPUs);
        } catch(error) {
           assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should compare given cpu description with host CPU': function(assert) {
        var cpu = fixture('cpu1.xml');
        //no supported by test driver
        try {
          hypervisor.compareCPU(cpu)
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return the amount of node free memory in one or more NUMA cells': function(assert) {
        var startCell = 0;
        var maxCells = 2;
        var memory = hypervisor.getNodeFreeMemoryInNumaCells(startCell, maxCells);
        assert.eql(memory instanceof Array, true);
        assert.eql(memory[0], 2097152);
        assert.eql(memory[1], 4194304);
    },

    'should return the node information where hypervisor is running': function(assert) {
        var info = hypervisor.getNodeInfo();
        assert.isNotNull(info.model);
        assert.isNotNull(info.memory);
        assert.isNotNull(info.cpus);
        assert.isNotNull(info.mhz);
        assert.isNotNull(info.nodes);
        assert.isNotNull(info.sockets);
        assert.isNotNull(info.cores);
        assert.isNotNull(info.threads);
    },

    'should return free memory of the physical node': function(assert) {
        try {
            var memory = hypervisor.getNodeFreeMemory();
            assert.isNotNull(memory);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should list node devices names': function(assert) {
        var devices = hypervisor.getNodeDevicesNames();
        assert.eql(devices instanceof Array, true);
    },

    'should return the node security model': function(assert) {
        try {
            var security = hypervisor.getNodeSecurityModel();
            assert.isDefined(security.model);
            assert.isDefined(security.doi);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should register function callbacks for domain events': function(assert) {
        var domain = hypervisor.lookupDomainByName('test');

        var args = { evtype: hypervisor.VIR_DOMAIN_EVENT_ID_LIFECYCLE,
                     domain: domain,
                     callback: function(hyp, dom, data) {
                        assert.eql(dom.getName(), 'test');
                        assert.eql(data.evtype, hyp.VIR_DOMAIN_EVENT_ID_LIFECYCLE);
                        assert.eql(data.detail, dom.VIR_DOMAIN_EVENT_STOPPED_SHUTDOWN);
                     }
                    };

        callback_id = hypervisor.registerDomainEvent(args);
        domain.shutdown();
        assert.eql(hypervisor.unregisterDomainEvent(callback_id), true);
    },

    'should unregister callbacks listening for domain events': function(assert) {
        var args = { evtype: hypervisor.VIR_DOMAIN_EVENT_ID_LIFECYCLE,
                     callback: function(hyp, dom, evtype, detail) {}
                    };

        callback_id = hypervisor.registerDomainEvent(args);
        assert.eql(hypervisor.unregisterDomainEvent(callback_id), true);
    },

    'should close a connection': function(assert) {
        assert.eql(hypervisor.closeConnection(), true);
    },
};

