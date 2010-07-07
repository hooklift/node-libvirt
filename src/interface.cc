// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include "interface.h"

namespace NodeLibvirt {
    Persistent<FunctionTemplate> Interface::constructor_template;

    void Interface::Initialize() {
        Local<FunctionTemplate> t = FunctionTemplate::New();

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "start",
                                      Interface::Start);
        NODE_SET_PROTOTYPE_METHOD(t, "stop",
                                      Interface::Stop);
        NODE_SET_PROTOTYPE_METHOD(t, "getName",
                                      Interface::GetName);
        NODE_SET_PROTOTYPE_METHOD(t, "getMacAddress",
                                      Interface::GetMacAddress);
        NODE_SET_PROTOTYPE_METHOD(t, "isActive",
                                      Interface::IsActive);
        NODE_SET_PROTOTYPE_METHOD(t, "undefine",
                                      Interface::Undefine);
        NODE_SET_PROTOTYPE_METHOD(t, "toXml",
                                      Interface::ToXml);

        Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

        //Constants
        //virInterfaceXMLFlags
        NODE_DEFINE_CONSTANT(object_tmpl, VIR_INTERFACE_XML_INACTIVE);


        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->SetClassName(String::NewSymbol("Interface"));
    }

    Handle<Value> Interface::Start(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        unsigned int flags = 0;

        Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());

        ret = virInterfaceCreate(interface->interface_, flags);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Interface::Stop(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        unsigned int flags = 0;

        Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());

        ret = virInterfaceDestroy(interface->interface_, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        if(interface->interface_ != NULL) {
            virInterfaceFree(interface->interface_);
        }

        return True();
    }

    Handle<Value> Interface::Define(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to call this function")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value xml(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Interface *interface = new Interface();
        interface->interface_ = virInterfaceDefineXML(hypervisor->connection(), (const char *) *xml, flags);

        if(interface->interface_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> interface_obj = interface->constructor_template->GetFunction()->NewInstance();

        interface->Wrap(interface_obj);

        return scope.Close(interface_obj);
    }

    Handle<Value> Interface::Undefine(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());
        ret = virInterfaceUndefine(interface->interface_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Interface::LookupByName(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid Interface name.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value name(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Interface *interface = new Interface();
        interface->interface_ = virInterfaceLookupByName(hypervisor->connection(), (const char *) *name);

        if(interface->interface_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> interface_obj = interface->constructor_template->GetFunction()->NewInstance();

        interface->Wrap(interface_obj);

        return scope.Close(interface_obj);
    }

    Handle<Value> Interface::LookupByMacAddress(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to call this function")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value mac(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Interface *interface = new Interface();
        interface->interface_ = virInterfaceLookupByMACString(hypervisor->connection(), (const char *) *mac);

        if(interface->interface_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> interface_obj = interface->constructor_template->GetFunction()->NewInstance();

        interface->Wrap(interface_obj);

        return scope.Close(interface_obj);
    }

    Handle<Value> Interface::GetName(const Arguments& args) {
        HandleScope scope;
        const char *name = NULL;

        Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());

        name = virInterfaceGetName(interface->interface_);

        if(name == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(String::New(name));
    }

    Handle<Value> Interface::GetMacAddress(const Arguments& args) {
        HandleScope scope;
        const char *mac = NULL;

        Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());

        mac = virInterfaceGetMACString(interface->interface_);

        if(mac == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(String::New(mac));
    }

    Handle<Value> Interface::IsActive(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());

        ret = virInterfaceIsActive(interface->interface_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        bool is_active = ret == 1 ? true : false;

        return scope.Close(Boolean::New(is_active));
    }

    Handle<Value> Interface::ToXml(const Arguments& args) {
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

        Interface *interface = ObjectWrap::Unwrap<Interface>(args.This());
        xml_ = virInterfaceGetXMLDesc(interface->interface_, flags);

        if(xml_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> xml = String::New(xml_);

        free(xml_);

        return scope.Close(xml);
    }
}

