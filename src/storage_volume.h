// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_STORAGE_VOLUME_H_
#define SRC_STORAGE_VOLUME_H_

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {

    class StorageVolume : public EventEmitter {
        friend class Hypervisor;
        friend class StoragePool;

        public:
            static void Initialize();
            static inline bool HasInstance(v8::Handle<v8::Value> value) {
                if (!value->IsObject()) {
                    return false;
                }
                v8::Local<v8::Object> object = value->ToObject();
                return constructor_template->HasInstance(object);
            }

        protected:
            static Handle<Value> Create(const Arguments& args);
            static Handle<Value> GetInfo(const Arguments& args);
            static Handle<Value> GetKey(const Arguments& args);
            static Handle<Value> GetName(const Arguments& args);
            static Handle<Value> GetPath(const Arguments& args);
            static Handle<Value> ToXml(const Arguments& args);
            static Handle<Value> LookupByKey(const Arguments& args);
            static Handle<Value> LookupByName(const Arguments& args);
            static Handle<Value> LookupByPath(const Arguments& args);
            static Handle<Value> Wipe(const Arguments& args);
            static Handle<Value> Delete(const Arguments& args);
            static Handle<Value> Clone(const Arguments& args);

        private:
            virStorageVolPtr volume_;
            static Persistent<FunctionTemplate> constructor_template;
    };

}  //namespace NodeLibvirt

#endif  // SRC_STORAGE_VOLUME_H

