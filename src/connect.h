// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_CONNECT_H_
#define SRC_CONNECT_H_

#include "node_libvirt.h"

namespace NodeLibvirt {

    class Connection : public EventEmitter {
        public:
            static void Initialize(v8::Handle<v8::Object> target);
            
        protected:
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetBaselineCPU(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHypCapabilities(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHypHostname(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRemoteLibVirtVersion(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMaxVcpus(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHypervisorType(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHypervisorUri(const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHypervisorVersion(const v8::Arguments& args);
            static v8::Handle<v8::Value> IsEncrypted(const v8::Arguments& args);
            static v8::Handle<v8::Value> IsSecure(const v8::Arguments& args);
                                                
    };

}  // namespace NodeLibvirt

#endif  // SRC_CONNECT_H_
