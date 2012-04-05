// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_SECRET_H_
#define SRC_SECRET_H_

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {

    class Secret : public ObjectWrap {
        friend class Hypervisor;

        public:
            static void Initialize();

        protected:
            static Handle<Value> Define(const Arguments& args);
            static Handle<Value> Undefine(const Arguments& args);
            static Handle<Value> LookupByUsage(const Arguments& args);
            static Handle<Value> LookupByUUID(const Arguments& args);
            static Handle<Value> GetUUID(const Arguments& args);
            static Handle<Value> GetUsageId(const Arguments& args);
            static Handle<Value> GetUsageType(const Arguments& args);
            static Handle<Value> GetValue(const Arguments& args);
            static Handle<Value> SetValue(const Arguments& args);
            static Handle<Value> ToXml(const Arguments& args);

        private:
            virSecretPtr secret_;
            static Persistent<FunctionTemplate> constructor_template;
    };

}  // namespace NodeLibvirt

#endif  // SRC_SECRET_H_

