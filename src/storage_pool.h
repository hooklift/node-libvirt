// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_STORAGE_POOL_H_
#define SRC_STORAGE_POOL_H_

#include "node_libvirt.h"
#include "hypervisor.h"
#include "error.h"

namespace NodeLibvirt {

    class StoragePool : public EventEmitter {
        friend class Hypervisor;

        public:
            static void Initialize();

        protected:
            static Handle<Value> Build(const Arguments& args);
            static Handle<Value> Create(const Arguments& args);
            static Handle<Value> Define(const Arguments& args);
            static Handle<Value> Undefine(const Arguments& args);
            static Handle<Value> Start(const Arguments& args);
            static Handle<Value> Stop(const Arguments& args);
            static Handle<Value> Erase(const Arguments& args);
            static Handle<Value> GetAutostart(const Arguments& args);
            static Handle<Value> SetAutostart(const Arguments& args);
            static Handle<Value> GetInfo(const Arguments& args);
            static Handle<Value> GetName(const Arguments& args);
            static Handle<Value> GetUUID(const Arguments& args);
            static Handle<Value> LookupByName(const Arguments& args);
            static Handle<Value> LookupByUUID(const Arguments& args);
            static Handle<Value> LookupByVolume(const Arguments& args);
            static Handle<Value> ToXml(const Arguments& args);
            static Handle<Value> IsActive(const Arguments& args);
            static Handle<Value> IsPersistent(const Arguments& args);
            static Handle<Value> GetVolumes(const Arguments& args);
            static Handle<Value> Refresh(const Arguments& args);

        private:
            virStoragePoolPtr pool_;
            static Persistent<FunctionTemplate> constructor_template;
    };

}  //namespace NodeLibvirt

#endif  // SRC_STORAGE_POOL_H

