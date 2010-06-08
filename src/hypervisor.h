// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_HYPERVISOR_H_
#define SRC_HYPERVISOR_H_

#include "node_libvirt.h"
#include "domain.h"

#define GET_LIST_OF(numof_function, list_function)                      \
({ \
   \
   \
    char **_names = NULL;                                               \
    int numInactiveItems;                                               \
                                                                        \
    numInactiveItems = numof_function(conn_);                            \
                                                                        \
    if(numInactiveItems == -1) {                                        \
        virError *error = virGetLastError();                            \
        if(error != NULL) {                                             \
            LIBVIRT_THROW_EXCEPTION(error->message);                    \
        }                                                               \
        return Null();                                                  \
    }                                                                   \
                                                                        \
    _names = (char **)malloc(sizeof(*_names) * numInactiveItems);       \
    if(_names == NULL) {                                                \
        LIBVIRT_THROW_EXCEPTION("unable to allocate memory");           \
        return Null();                                                  \
    }                                                                   \
                                                                        \
    int ret = list_function(conn_, _names, numInactiveItems);            \
                                                                        \
    if(ret == -1) {                                                     \
        virError *error = virGetLastError();                            \
        if(error != NULL) {                                             \
            free(_names);                                               \
            LIBVIRT_THROW_EXCEPTION(error->message);                    \
            return Null();                                              \
        }                                                               \
    }                                                                   \
                                                                        \
    TO_V8_ARRAY(numInactiveItems, _names);                              \
})

#define GET_NUM_OF(function)                            \
({                                                      \
    int ret = function(conn_);                           \
    if(ret == -1) {                                    \
        virError *error = virGetLastError();            \
        if(error != NULL) {                             \
            LIBVIRT_THROW_EXCEPTION(error->message);    \
        }                                               \
        return Null();                                  \
    }                                                   \
    return Integer::New(ret);                           \
})                                                      \

namespace NodeLibvirt {

    class Hypervisor : public EventEmitter {
        public:
            static void Initialize(Handle<Object> target);
            //static inline bool HasInstance(v8::Handle<v8::Value> value);
            static inline bool HasInstance(v8::Handle<v8::Value> value) {
                if (!value->IsObject()) {
                    return false;
                }
                v8::Local<v8::Object> object = value->ToObject();
                return constructor_template->HasInstance(object);
            }
            virConnectPtr connection() const;
        protected:
            static Handle<Value> New(const Arguments& args);

            static Handle<Value> GetCapabilities(const Arguments& args);
            static Handle<Value> GetHostname(const Arguments& args);
            static Handle<Value> GetType(const Arguments& args);
            static Handle<Value> GetConnectionUri(const Arguments& args);
            static Handle<Value> GetVersion(const Arguments& args);
            static Handle<Value> GetLibVirtVersion(const Arguments& args);
            static Handle<Value> GetMaxVcpus(const Arguments& args);
            static Handle<Value> GetBaselineCPU(const Arguments& args);
            static Handle<Value> CompareCPU(const Arguments& args);
            static Handle<Value> IsConnectionEncrypted(const Arguments& args);
            static Handle<Value> IsConnectionSecure(const Arguments& args);
            static Handle<Value> CloseConnection(const Arguments& args);

            //virConnectList functions
            static Handle<Value> GetDefinedDomains(const Arguments& args);
            static Handle<Value> GetDefinedInterfaces(const Arguments& args);
            static Handle<Value> GetDefinedNetworks(const Arguments& args);
            static Handle<Value> GetDefinedStoragePools(const Arguments& args);
            static Handle<Value> GetActiveDomains(const Arguments& args);
            static Handle<Value> GetActiveInterfaces(const Arguments& args);
            static Handle<Value> GetNetworkFilters(const Arguments& args);
            static Handle<Value> GetActiveNetworks(const Arguments& args);
            static Handle<Value> GetSecrets(const Arguments& args);
            static Handle<Value> GetActiveStoragePools(const Arguments& args);

            //virConnectNumOf functions
            static Handle<Value> GetNumberOfDefinedDomains(const Arguments& args);
            static Handle<Value> GetNumberOfDefinedInterfaces(const Arguments& args);
            static Handle<Value> GetNumberOfDefinedNetworks(const Arguments& args);
            static Handle<Value> GetNumberOfDefinedStoragePools(const Arguments& args);
            static Handle<Value> GetNumberOfActiveDomains(const Arguments& args);
            static Handle<Value> GetNumberOfActiveInterfaces(const Arguments& args);
            static Handle<Value> GetNumberOfActiveNetworks(const Arguments& args);
            static Handle<Value> GetNumberOfNetworkFilters(const Arguments& args);
            static Handle<Value> GetNumberOfSecrets(const Arguments& args);
            static Handle<Value> GetNumberOfActiveStoragePools(const Arguments& args);

            //Domain functions
            static Handle<Value> CreateDomain(const Arguments& args);
            //static Handle<Value> DefineDomain(const Arguments& args);
            static Handle<Value> LookupDomainById(const Arguments& args);
            static Handle<Value> LookupDomainByName(const Arguments& args);
            static Handle<Value> LookupDomainByUUID(const Arguments& args);

            Hypervisor(const Local<String>& uri, bool readOnly);
            ~Hypervisor();

            Handle<Value> get_capabilities();
            Handle<Value> get_hostname();
            Handle<Value> get_type();
            Handle<Value> get_connection_uri();
            Handle<Value> get_version();
            Handle<Value> get_libvirt_version();
            Handle<Value> get_max_vcpus();
            Handle<Value> get_baseline_cpu( char **xmlCPUs, unsigned int ncpus,
                                            unsigned int flags);
            Handle<Value> compare_cpu(const char *xmlDesc, unsigned int flags);
            Handle<Value> is_connection_encrypted();
            Handle<Value> is_connection_secure();
            Handle<Value> close_connection();

            Handle<Value> get_defined_domains();
            Handle<Value> get_defined_interfaces();
            Handle<Value> get_defined_networks();
            Handle<Value> get_defined_storage_pools();
            Handle<Value> get_active_domains();
            Handle<Value> get_active_interfaces();
            Handle<Value> get_active_networks();
            Handle<Value> get_active_storage_pools();
            Handle<Value> get_network_filters();
            Handle<Value> get_secrets();

            Handle<Value> get_number_of_defined_domains();
            Handle<Value> get_number_of_defined_interfaces();
            Handle<Value> get_number_of_defined_networks();
            Handle<Value> get_number_of_defined_storage_pools();
            Handle<Value> get_number_of_active_domains();
            Handle<Value> get_number_of_active_interfaces();
            Handle<Value> get_number_of_active_networks();
            Handle<Value> get_number_of_active_storage_pools();
            Handle<Value> get_number_of_network_filters();
            Handle<Value> get_number_of_secrets();

            Handle<Value> create_domain(const Arguments& args);
            //Handle<Value> define_domain();
            Handle<Value> lookup_domain_by_id(const Arguments& args);
            Handle<Value> lookup_domain_by_name(const Arguments& args);
            Handle<Value> lookup_domain_by_uuid(const Arguments& args);

        private:
            virConnectPtr conn_;
            //static v8::Persistent<v8::FunctionTemplate> constructor_template;
    };

}  // namespace NodeLibvirt

#endif  // SRC_HYPERVISOR_H_

