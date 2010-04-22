// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_CONNECT_H_
#define SRC_CONNECT_H_

#include "node_libvirt.h"

namespace NodeLibvirt {

    class Connection : public EventEmitter {
        public:
            static void Initialize(Handle<Object> target);
            
        protected:
            static Handle<Value> New(const Arguments& args);
            
            static Handle<Value> GetHypervisorCapabilities(const Arguments& args);
            static Handle<Value> GetHypervisorHostname(const Arguments& args);
            static Handle<Value> GetHypervisorType(const Arguments& args);
            static Handle<Value> GetHypervisorUri(const Arguments& args);
            static Handle<Value> GetHypervisorVersion(const Arguments& args);
            static Handle<Value> GetRemoteLibVirtVersion(const Arguments& args);
            static Handle<Value> GetMaxVcpus(const Arguments& args);
            static Handle<Value> GetBaselineCPU(const Arguments& args);
            static Handle<Value> CompareCPU(const Arguments& args);
            static Handle<Value> IsEncrypted(const Arguments& args);
            static Handle<Value> IsSecure(const Arguments& args);
            static Handle<Value> Close(const Arguments& args);

            //virConnectList functions
            static Handle<Value> ListDefinedDomains(const Arguments& args);
            static Handle<Value> ListDefinedInterfaces(const Arguments& args);
            static Handle<Value> ListDefinedNetworks(const Arguments& args);
            static Handle<Value> ListDefinedStoragePools(const Arguments& args);
            static Handle<Value> ListDomains(const Arguments& args);
            static Handle<Value> ListInterfaces(const Arguments& args);
            static Handle<Value> ListNWFilters(const Arguments& args);
            static Handle<Value> ListNetworks(const Arguments& args);
            static Handle<Value> ListSecrets(const Arguments& args);
            static Handle<Value> ListStoragePools(const Arguments& args);
            
            //virConnectNumOf functions
            static Handle<Value> GetNumOfDefinedDomains(const Arguments& args);
            static Handle<Value> GetNumOfDefinedInterfaces(const Arguments& args);
            static Handle<Value> GetNumOfDefinedNetworks(const Arguments& args);
            static Handle<Value> GetNumOfDefinedStoragePools(const Arguments& args);
            static Handle<Value> GetNumOfDomains(const Arguments& args);
            static Handle<Value> GetNumOfInterfaces(const Arguments& args);
            static Handle<Value> GetNumOfNetworks(const Arguments& args);
            static Handle<Value> GetNumOfNWFilters(const Arguments& args);
            static Handle<Value> GetNumOfSecrets(const Arguments& args);
            static Handle<Value> GetNumOfStoragePools(const Arguments& args);

            //TODO virConnectDomain functions
            
            Connection(const Local<String>& uri, bool readOnly);
            ~Connection();
            
            Handle<Value> get_hypervisor_capabilities();
            Handle<Value> get_hypervisor_hostname();
            Handle<Value> get_hypervisor_type();
            Handle<Value> get_hypervisor_uri();
            Handle<Value> get_hypervisor_version();
            Handle<Value> get_remote_libvirt_version();
            Handle<Value> get_max_vcpus();
            Handle<Value> get_baseline_cpu(char **xmlCPUs, unsigned int ncpus, unsigned int flags);
            Handle<Value> compare_cpu(const char *xmlDesc, unsigned int flags);
            Handle<Value> is_encrypted();
            Handle<Value> is_secure();
            Handle<Value> close();

            Handle<Value> list_defined_domains();
            
        private:
            virConnectPtr conn;  
    };

}  // namespace NodeLibvirt

#endif  // SRC_CONNECT_H_
