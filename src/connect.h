// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_CONNECT_H_
#define SRC_CONNECT_H_

#include "node_libvirt.h"

namespace NodeLibvirt {

    class Connection : public EventEmitter {
        public:
            static void Initialize(v8::Handle<v8::Object> target);
            //static v8::Persistent<v8::FunctionTemplate> constructor_template;
            
        protected:
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
            
            static v8::Handle<v8::Value> GetHypervisorCapabilities(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHypervisorHostname(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHypervisorType(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHypervisorUri(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHypervisorVersion(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRemoteLibVirtVersion(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMaxVcpus(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetBaselineCPU(const v8::Arguments& args);
            static v8::Handle<v8::Value> IsEncrypted(const v8::Arguments& args);
            static v8::Handle<v8::Value> IsSecure(const v8::Arguments& args);
            static v8::Handle<v8::Value> Close(const v8::Arguments& args);
            
            Connection(const v8::Local<v8::String>& uri, bool readOnly);
            ~Connection();
            v8::Handle<v8::Value> get_hypervisor_capabilities();
            v8::Handle<v8::Value> get_hypervisor_hostname();
            v8::Handle<v8::Value> get_hypervisor_type();
            v8::Handle<v8::Value> get_hypervisor_uri();
            v8::Handle<v8::Value> get_hypervisor_version();
            v8::Handle<v8::Value> get_remote_libvirt_version();
            v8::Handle<v8::Value> get_max_vcpus();
            v8::Handle<v8::Value> get_baseline_cpu(const char **xmlCPUs, unsigned int ncpus, unsigned int flags);            
            v8::Handle<Value> is_encrypted();
            v8::Handle<Value> is_secure();
            v8::Handle<Value> close();
            
        private:
            virConnectPtr conn;  
    };

}  // namespace NodeLibvirt

#endif  // SRC_CONNECT_H_
