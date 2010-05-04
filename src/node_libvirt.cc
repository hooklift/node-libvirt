#include <stdio.h>
#include <stdlib.h>
#include "node_libvirt.h"
#include "connect.h"
//#include "domain.h"
//#include "device.h"
//#include "network.h"
//#include "storage_pool.h"
//#include "storage_volume.h"

namespace NodeLibvirt {
    void InitializeLibvirt(Handle<Object> target) {
        HandleScope scope;
        
        Connection::Initialize(target);
        //Domain::Initialize(target);
        //Network::Initialize(target);
        //StoragePool::Initialize(target);
        //StorageVolume::Initialize(target);
        
        target->Set(String::NewSymbol("version"),
                    String::New(NODE_LIBVIRT_VERSION));
     
        target->Set(String::NewSymbol("libvirt_version"),
                    Number::New(LIBVIR_VERSION_NUMBER));
     
        Handle<ObjectTemplate> global = ObjectTemplate::New();
        Handle<Context> context = Context::New(NULL, global);
        Context::Scope context_scope(context);
        
        context->Global()->Set(String::NewSymbol("libvirt"), target);
    }
    
    // Extracts a C string from a V8 Utf8Value.
    const char* ToCString(const String::Utf8Value& value) {
        return *value ? *value : "<string conversion failed>";
    }

    /*Handle<Value> ToV8Array(int size, char **items) {
        Local<Array> v8Array = Array::New(size);
               
        for(int i = 0; i < size; i++) {                             
            v8Array->Set(Integer::New(i), String::New(items[i]));
            free(items[i]);
        }
        return v8Array;
    }*/

    extern "C" void init (Handle<Object> target) {
      HandleScope scope;
      InitializeLibvirt(target);
    }
} //namespace NodeLibvirt
