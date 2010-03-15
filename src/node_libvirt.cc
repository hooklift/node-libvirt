#include "node_libvirt.h"
#include "connect.h"
//#include "domain.h"
//#include "device.h"
//#include "network.h"
//#include "storage_pool.h"
//#include "storage_volume.h"

namespace NodeLibvirt {
    void InitializeLibvirt(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        
        Connection::Initialize(target);
    //    Domain::Initialize(target);
    //    Device::Initialize(target);
    //    Network::Initialize(target);
    //    StoragePool::Initialize(target);
    //    StorageVolume::Initialize(target);
        
        target->Set(v8::String::NewSymbol("version"),
                    v8::String::New(NODE_LIBVIRT_VERSION));
     
        target->Set(v8::String::NewSymbol("libvirt_version"),
                    v8::String::New(LIBVIRT_VERSION));
     
        v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
        v8::Handle<v8::Context> context = v8::Context::New(NULL, global);
        
        v8::Context::Scope context_scope(context);
        context->Global()->Set(v8::String::NewSymbol("libvirt"), target);
    }

    extern "C" void init (Handle<Object> target) {
      HandleScope scope;
      InitializeLibvirt(target);
    }
} //namespace NodeLibvirt
