// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_INTERFACE_H_
#define SRC_INTERFACE_H_

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {

    class Interface : public EventEmitter {
        friend class Hypervisor;

        public:
            static void Initialize();

        protected:
            static Handle<Value> Start(const Arguments& args);
            static Handle<Value> Stop(const Arguments& args);
            static Handle<Value> LookupByName(const Arguments& args);
            static Handle<Value> LookupByMacAddress(const Arguments& args);
            static Handle<Value> Define(const Arguments& args);
            static Handle<Value> Undefine(const Arguments& args);
            static Handle<Value> GetName(const Arguments& args);
            static Handle<Value> IsActive(const Arguments& args);
            static Handle<Value> ToXml(const Arguments& args);
            static Handle<Value> GetMacAddress(const Arguments& args);

        private:
            virInterfacePtr interface_;
            static Persistent<FunctionTemplate> constructor_template;
    };

}  // namespace NodeLibvirt

#endif  // SRC_INTERFACE_H_

