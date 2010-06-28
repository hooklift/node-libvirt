// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NODE_DEVICE_H_
#define SRC_NODE_DEVICE_H_

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {

    class NodeDevice : public EventEmitter {
        friend class Hypervisor;

        public:
            static void Initialize();
            static inline bool HasInstance(v8::Handle<v8::Value> value) {
                if (!value->IsObject()) {
                    return false;
                }
                v8::Local<v8::Object> object = value->ToObject();
                return constructor_template->HasInstance(object);
            }
            virtual ~NodeDevice();

        protected:
            static Handle<Value> Create(const Arguments& args);
            static Handle<Value> Destroy(const Arguments& args);
            static Handle<Value> LookupByName(const Arguments& args);
            static Handle<Value> Detach(const Arguments& args);
            static Handle<Value> Reattach(const Arguments& args);
            static Handle<Value> Reset(const Arguments& args);
            static Handle<Value> GetName(const Arguments& args);
            static Handle<Value> GetParentName(const Arguments& args);
            static Handle<Value> ToXml(const Arguments& args);
            static Handle<Value> GetCapabilities(const Arguments& args);

        private:
            virNodeDevicePtr device_;
            static Persistent<FunctionTemplate> constructor_template;
    };

}  //namespace NodeLibvirt

#endif  // SRC_NODE_DEVICE_H

