// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include "storage_pool.h"
//FIXME default params, default flags

namespace NodeLibvirt {
    Persistent<FunctionTemplate> StoragePool::constructor_template;

    //storage pool info
    static Persistent<String> info_state_symbol;
    static Persistent<String> info_capacity_symbol;
    static Persistent<String> info_allocation_symbol;
    static Persistent<String> info_available_symbol;

    void StoragePool::Initialize() {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New();

//        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "build",
                                      StoragePool::Build);
        NODE_SET_PROTOTYPE_METHOD(t, "start",
                                      StoragePool::Start);
        NODE_SET_PROTOTYPE_METHOD(t, "stop",
                                      StoragePool::Stop);
        NODE_SET_PROTOTYPE_METHOD(t, "erase",
                                      StoragePool::Erase);
        NODE_SET_PROTOTYPE_METHOD(t, "setAutostart",
                                      StoragePool::SetAutostart);
        NODE_SET_PROTOTYPE_METHOD(t, "getAutostart",
                                      StoragePool::GetAutostart);
        NODE_SET_PROTOTYPE_METHOD(t, "getInfo",
                                      StoragePool::GetInfo);
        NODE_SET_PROTOTYPE_METHOD(t, "getName",
                                      StoragePool::GetName);
        NODE_SET_PROTOTYPE_METHOD(t, "getUUID",
                                      StoragePool::GetUUID);
        NODE_SET_PROTOTYPE_METHOD(t, "undefine",
                                      StoragePool::Undefine);
        NODE_SET_PROTOTYPE_METHOD(t, "toXml",
                                      StoragePool::ToXml);
        NODE_SET_PROTOTYPE_METHOD(t, "isActive",
                                      StoragePool::IsActive);
        NODE_SET_PROTOTYPE_METHOD(t, "isPersistent",
                                      StoragePool::IsPersistent);
        NODE_SET_PROTOTYPE_METHOD(t, "getVolumes",
                                      StoragePool::GetVolumes);
        NODE_SET_PROTOTYPE_METHOD(t, "refresh",
                                      StoragePool::Refresh);
        NODE_SET_PROTOTYPE_METHOD(t, "createVolume",
                                      StorageVolume::Create);
        NODE_SET_PROTOTYPE_METHOD(t, "cloneVolume",
                                      StorageVolume::Clone);
        NODE_SET_PROTOTYPE_METHOD(t, "lookupVolumeByName",
                                      StorageVolume::LookupByName);

        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->SetClassName(String::NewSymbol("StoragePool"));

        Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

        //Constants initialization
        //virStoragePoolDeleteFlags
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_DELETE_NORMAL);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_DELETE_ZEROED);

        //virStoragePoolState
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_INACTIVE);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_BUILDING);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_RUNNING);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_DEGRADED);
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_STORAGE_POOL_INACCESSIBLE);

        info_state_symbol        = NODE_PSYMBOL("state");
        info_capacity_symbol     = NODE_PSYMBOL("capacity");
        info_allocation_symbol   = NODE_PSYMBOL("allocation");
        info_available_symbol    = NODE_PSYMBOL("available");
    }

    virStoragePoolPtr StoragePool::pool() const {
        return pool_;
    }

    Handle<Value> StoragePool::Build(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolBuild(pool->pool_, flags);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> StoragePool::Create(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;

        int argsl = args.Length();

        if(argsl == 0) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at least one argument")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }
        String::Utf8Value xml(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        StoragePool *pool = new StoragePool();
        pool->pool_ = virStoragePoolCreateXML(hypervisor->connection(), (const char *) *xml, flags);

        if(pool->pool_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> pool_obj = pool->constructor_template->GetFunction()->NewInstance();
        pool->Wrap(pool_obj);

        return scope.Close(pool_obj);
    }

    Handle<Value> StoragePool::Define(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;

        int argsl = args.Length();

        if(argsl == 0) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify at least one argument")));
        }

        if(!args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value xml(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        StoragePool *pool = new StoragePool();
        pool->pool_ = virStoragePoolDefineXML(hypervisor->connection(), (const char *) *xml, flags);

        if(pool->pool_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> pool_obj = pool->constructor_template->GetFunction()->NewInstance();

        pool->Wrap(pool_obj);

        return scope.Close(pool_obj);
    }

    Handle<Value> StoragePool::Undefine(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
        ret = virStoragePoolUndefine(pool->pool_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        if(pool->pool_ != NULL) {
            virStoragePoolFree(pool->pool_);
        }

        return True();
    }

    Handle<Value> StoragePool::LookupByName(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument to call this function")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value name(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        StoragePool *pool = new StoragePool();
        pool->pool_ = virStoragePoolLookupByName(hypervisor->connection(), (const char *) *name);

        if(pool->pool_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> pool_obj = pool->constructor_template->GetFunction()->NewInstance();

        pool->Wrap(pool_obj);

        return scope.Close(pool_obj);
    }

    Handle<Value> StoragePool::LookupByUUID(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a UUID string.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value uuid(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        StoragePool *pool = new StoragePool();
        pool->pool_ = virStoragePoolLookupByUUIDString(hypervisor->connection(), (const char *) *uuid);

        if(pool->pool_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> pool_obj = pool->constructor_template->GetFunction()->NewInstance();

        pool->Wrap(pool_obj);

        return scope.Close(pool_obj);
    }

    Handle<Value> StoragePool::LookupByVolume(const Arguments& args) {
        HandleScope scope;
        return Undefined();
    }

    Handle<Value> StoragePool::Start(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolCreate(pool->pool_, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }
        return True();
    }

    Handle<Value> StoragePool::Stop(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolDestroy(pool->pool_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        /*if(pool->pool_ != NULL) {
            virStoragePoolFree(pool->pool_);
        }*/

        return True();
    }

    Handle<Value> StoragePool::Erase(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        int args_len = args.Length();

        if(args_len == 1) {
            if(!args[0]->IsArray()) {
                return ThrowException(Exception::TypeError(
                String::New("You must specify an array as argument to call this function")));
            }

            //flags
            Local<Array> flags_ = Local<Array>::Cast(args[0]);
            unsigned int length = flags_->Length();

            for (unsigned int i = 0; i < length; i++) {
                flags |= flags_->Get(Integer::New(i))->Int32Value();
            }
        }

        if(flags == 0) {
            flags |= VIR_STORAGE_POOL_DELETE_NORMAL;
        }

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolDelete(pool->pool_, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        if(pool->pool_ != NULL) {
            virStoragePoolFree(pool->pool_);
        }

        return True();
    }

    Handle<Value> StoragePool::GetAutostart(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        int autostart_;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolGetAutostart(pool->pool_, &autostart_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        bool autostart = autostart_ == 0 ? true : false;

        return scope.Close(Boolean::New(autostart));
    }

    Handle<Value> StoragePool::SetAutostart(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsBoolean()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a boolean argument")));
        }

        bool autostart = args[0]->IsTrue();

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolSetAutostart(pool->pool_, autostart ? 0 : 1);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> StoragePool::GetInfo(const Arguments& args) {
        HandleScope scope;
        virStoragePoolInfo info;
        int ret = -1;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolGetInfo(pool->pool_, &info);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        Local<Object> object = Object::New();

        object->Set(info_state_symbol, Integer::New(info.state)); //virStoragePoolState
        object->Set(info_capacity_symbol, Number::New(info.capacity)); //bytes
        object->Set(info_allocation_symbol, Number::New(info.allocation)); //bytes
        object->Set(info_available_symbol, Number::New(info.available)); //bytes

        return scope.Close(object);
    }

    Handle<Value> StoragePool::GetName(const Arguments& args) {
        HandleScope scope;
        const char *name = NULL;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        name = virStoragePoolGetName(pool->pool_);

        if(name == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(String::New(name));
    }

    Handle<Value> StoragePool::GetUUID(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        char *uuid = new char[VIR_UUID_STRING_BUFLEN];

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolGetUUIDString(pool->pool_, uuid);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            delete[] uuid;
            return Null();
        }

        Local<String> uuid_str = String::New(uuid);

        delete[] uuid;

        return scope.Close(uuid_str);
    }

    Handle<Value> StoragePool::ToXml(const Arguments& args) {
        HandleScope scope;
        char* xml_ = NULL;
        int flags = 0;

        if(args.Length() == 0 || !args[0]->IsArray()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify an array as argument to invoke this function")));
        }

        //flags
        Local<Array> flags_ = Local<Array>::Cast(args[0]);
        unsigned int length = flags_->Length();

        for (unsigned int i = 0; i < length; i++) {
            flags |= flags_->Get(Integer::New(i))->Int32Value();
        }

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());
        xml_ = virStoragePoolGetXMLDesc(pool->pool_, flags);

        if(xml_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> xml = String::New(xml_);

        free(xml_);

        return scope.Close(xml);
    }

    Handle<Value> StoragePool::IsActive(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolIsActive(pool->pool_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        bool is_active = ret == 1 ? true : false;

        return scope.Close(Boolean::New(is_active));
    }

    Handle<Value> StoragePool::IsPersistent(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolIsPersistent(pool->pool_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        bool is_persistent = ret == 1 ? true : false;

        return scope.Close(Boolean::New(is_persistent));
    }

    Handle<Value> StoragePool::GetVolumes(const Arguments& args) {
        HandleScope scope;
        char **volumes_ = NULL;
        int num_volumes = 0;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        num_volumes = virStoragePoolNumOfVolumes(pool->pool_);

        if(num_volumes == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        volumes_ = (char**) malloc(sizeof(*volumes_) * num_volumes);
        if(volumes_ == NULL) {
            LIBVIRT_THROW_EXCEPTION("unable to allocate memory");
            return Null();
        }

        num_volumes = virStoragePoolListVolumes(pool->pool_, volumes_, num_volumes);
        if(num_volumes == -1) {
            free(volumes_);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Array> volumes = Array::New(num_volumes);
        for (int i = 0; i < num_volumes; i++) {
            //TODO new StorageVolume and return array of StorageVolume instances
            volumes->Set(Integer::New(i), String::New(volumes_[i]));
            free(volumes_[i]);
        }
        free(volumes_);

        return scope.Close(volumes);
    }

    Handle<Value> StoragePool::Refresh(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        StoragePool *pool = ObjectWrap::Unwrap<StoragePool>(args.This());

        ret = virStoragePoolRefresh(pool->pool_, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

} //namespace NodeLibvirt

