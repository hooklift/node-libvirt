// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_HYPERVISOR_H_
#define SRC_HYPERVISOR_H_

#include "node_libvirt.h"
#include "domain.h"
#include "node_device.h"
#include "network.h"
#include "network_filter.h"
#include "interface.h"
#include "error.h"

namespace NodeLibvirt {

    class Hypervisor : public EventEmitter {
        public:
            static void Initialize(Handle<Object> target);
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

            //Node functions
            static Handle<Value> GetNodeFreeMemoryInNumaCells(const Arguments& args);
            static Handle<Value> GetNodeFreeMemory(const Arguments& args);
            static Handle<Value> GetNodeInfo(const Arguments& args);
            static Handle<Value> GetNodeDevicesNames(const Arguments& args);
            static Handle<Value> GetNodeSecurityModel(const Arguments& args);

            Hypervisor(const Local<String>& uri, bool readOnly);

        private:
            virConnectPtr conn_;
    };

}  // namespace NodeLibvirt

#endif  // SRC_HYPERVISOR_H_

