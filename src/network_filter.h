// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NETWORK_FILTER_H_
#define SRC_NETWORK_FILTER_H_

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {

    class NetworkFilter : public EventEmitter {
        friend class Hypervisor;

        public:
            static void Initialize();

        protected:
            static Handle<Value> LookupByName(const Arguments& args);
            static Handle<Value> LookupByUUID(const Arguments& args);
            static Handle<Value> Define(const Arguments& args);
            static Handle<Value> Undefine(const Arguments& args);
            static Handle<Value> GetName(const Arguments& args);
            static Handle<Value> GetUUID(const Arguments& args);
            static Handle<Value> ToXml(const Arguments& args);

            ~NetworkFilter();

        private:
            virNWFilterPtr filter_;
            static Persistent<FunctionTemplate> constructor_template;
    };

}  //namespace NodeLibvirt

#endif  // SRC_NETWORK_FILTER_H_

