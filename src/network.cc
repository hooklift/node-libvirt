// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include "network.h"

namespace NodeLibvirt {
    Persistent<FunctionTemplate> Network::constructor_template;

    void Network::Initialize() {
        Local<FunctionTemplate> t = FunctionTemplate::New();

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "start",
                                      Network::Start);
        NODE_SET_PROTOTYPE_METHOD(t, "getName",
                                      Network::GetName);
        NODE_SET_PROTOTYPE_METHOD(t, "getUUID",
                                      Network::GetUUID);
        NODE_SET_PROTOTYPE_METHOD(t, "getAutostart",
                                      Network::GetAutostart);
        NODE_SET_PROTOTYPE_METHOD(t, "setAutostart",
                                      Network::SetAutostart);
        NODE_SET_PROTOTYPE_METHOD(t, "isActive",
                                      Network::IsActive);
        NODE_SET_PROTOTYPE_METHOD(t, "isPersistent",
                                      Network::IsPersistent);
        NODE_SET_PROTOTYPE_METHOD(t, "undefine",
                                      Network::Undefine);
        NODE_SET_PROTOTYPE_METHOD(t, "destroy",
                                      Network::Destroy);
        NODE_SET_PROTOTYPE_METHOD(t, "toXml",
                                      Network::ToXml);
        NODE_SET_PROTOTYPE_METHOD(t, "getBridgeName",
                                      Network::GetBridgeName);

        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->SetClassName(String::NewSymbol("Network"));
    }

    Handle<Value> Network::Create(const Arguments& args) {
        HandleScope scope;
        const char* xml = NULL;

        Local<Object> hyp_obj = args.This();
        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor object instance")));
        }

        if(args.Length() == 1 && !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as first argument")));
        }
        String::Utf8Value xml_(args[0]->ToString());
        xml = ToCString(xml_);

        Network *network = new Network();
        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        network->network_ = virNetworkCreateXML(hypervisor->connection(), xml);

        if(network->network_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> network_obj = network->constructor_template->GetFunction()->NewInstance();
        network->Wrap(network_obj);

        return scope.Close(network_obj);
    }

    Handle<Value> Network::Start(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Network *network = ObjectWrap::Unwrap<Network>(args.This());

        ret = virNetworkCreate(network->network_);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Network::LookupByName(const Arguments& args) {
        HandleScope scope;
        const char* name = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid Network name.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value name_(args[0]->ToString());

        name = ToCString(name_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Network *network = new Network();
        network->network_ = virNetworkLookupByName(hypervisor->connection(), name);

        if(network->network_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> network_obj = network->constructor_template->GetFunction()->NewInstance();

        network->Wrap(network_obj);

        return scope.Close(network_obj);
    }

    Handle<Value> Network::LookupByUUID(const Arguments& args) {
        HandleScope scope;
        const char* uuid = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a UUID string.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value uuid_(args[0]->ToString());

        uuid = ToCString(uuid_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Network *network = new Network();
        network->network_ = virNetworkLookupByUUIDString(hypervisor->connection(), uuid);

        if(network->network_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> network_obj = network->constructor_template->GetFunction()->NewInstance();

        network->Wrap(network_obj);

        return scope.Close(network_obj);
    }

    Handle<Value> Network::Define(const Arguments& args) {
        HandleScope scope;
        const char* xml = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to call this function")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value xml_(args[0]->ToString());
        xml = ToCString(xml_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Network *network = new Network();
        network->network_ = virNetworkDefineXML(hypervisor->connection(), xml);

        if(network->network_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> network_obj = network->constructor_template->GetFunction()->NewInstance();

        network->Wrap(network_obj);

        return scope.Close(network_obj);
    }

    Handle<Value> Network::GetName(const Arguments& args) {
        HandleScope scope;
        const char *name = NULL;

        Network *network = ObjectWrap::Unwrap<Network>(args.This());

        name = virNetworkGetName(network->network_);

        if(name == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return String::New(name);
    }

    Handle<Value> Network::GetUUID(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        char *uuid = new char[VIR_UUID_STRING_BUFLEN];

        Network *network = ObjectWrap::Unwrap<Network>(args.This());

        ret = virNetworkGetUUIDString(network->network_, uuid);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            delete[] uuid;
            return Null();
        }

        Local<String> uuid_str = String::New(uuid);

        delete[] uuid;

        return uuid_str;
    }

     Handle<Value> Network::GetAutostart(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        int autostart_;

        Network *network = ObjectWrap::Unwrap<Network>(args.This());

        ret = virNetworkGetAutostart(network->network_, &autostart_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        bool autostart = autostart_ == 0 ? true : false;

        return Boolean::New(autostart);
    }

    Handle<Value> Network::SetAutostart(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsBoolean()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a boolean argument")));
        }

        bool autostart = args[0]->IsTrue();

        Network *network = ObjectWrap::Unwrap<Network>(args.This());

        ret = virNetworkSetAutostart(network->network_, autostart ? 0 : 1);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Network::IsActive(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Network *network = ObjectWrap::Unwrap<Network>(args.This());

        ret = virNetworkIsActive(network->network_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        bool is_active = ret == 1 ? true : false;

        return Boolean::New(is_active);
    }

    Handle<Value> Network::IsPersistent(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Network *network = ObjectWrap::Unwrap<Network>(args.This());

        ret = virNetworkIsPersistent(network->network_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        bool is_persistent = ret == 1 ? true : false;

        return Boolean::New(is_persistent);
    }

    Handle<Value> Network::Undefine(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Network *network = ObjectWrap::Unwrap<Network>(args.This());
        ret = virNetworkUndefine(network->network_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

     //Really neccesary call destroy from javascript ???
    Handle<Value> Network::Destroy(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Network *network = ObjectWrap::Unwrap<Network>(args.This());

        ret = virNetworkDestroy(network->network_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        if(network->network_ != NULL) {
            virNetworkFree(network->network_);
        }

        return True();
    }

    Handle<Value> Network::ToXml(const Arguments& args) {
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

        for (int i = 0; i < length; i++) {
            flags |= flags_->Get(Integer::New(i))->Int32Value();
        }

        Network *network = ObjectWrap::Unwrap<Network>(args.This());
        xml_ = virNetworkGetXMLDesc(network->network_, flags);

        if(xml_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> xml = String::New(xml_);

        free(xml_);

        return xml;
    }

    Handle<Value> Network::GetBridgeName(const Arguments& args) {
        HandleScope scope;
        const char *name = NULL;

        Network *network = ObjectWrap::Unwrap<Network>(args.This());

        name = virNetworkGetBridgeName(network->network_);

        if(name == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return String::New(name);
    }
}

