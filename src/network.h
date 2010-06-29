// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NETWORK_H_
#define SRC_NETWORK_H_

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {

    class Network : public EventEmitter {
        friend class Hypervisor;

        public:
            static void Initialize();

        protected:
            static Handle<Value> Create(const Arguments& args);
            static Handle<Value> Start(const Arguments& args);
            static Handle<Value> LookupByName(const Arguments& args);
            static Handle<Value> LookupByUUID(const Arguments& args);
            static Handle<Value> Define(const Arguments& args);
            static Handle<Value> Undefine(const Arguments& args);
            static Handle<Value> GetName(const Arguments& args);
            static Handle<Value> GetUUID(const Arguments& args);
            static Handle<Value> GetAutostart(const Arguments& args);
            static Handle<Value> SetAutostart(const Arguments& args);
            static Handle<Value> IsActive(const Arguments& args);
            static Handle<Value> IsPersistent(const Arguments& args);
            static Handle<Value> ToXml(const Arguments& args);
            static Handle<Value> GetBridgeName(const Arguments& args);
            static Handle<Value> Destroy(const Arguments& args);

            ~Network();

        private:
            virNetworkPtr network_;
            static Persistent<FunctionTemplate> constructor_template;
    };

}  // namespace NodeLibvirt

#endif  // SRC_NETWORK_H_

