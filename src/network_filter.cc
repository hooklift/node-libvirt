// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include "network_filter.h"

namespace NodeLibvirt {
    Persistent<FunctionTemplate> NetworkFilter::constructor_template;

    void NetworkFilter::Initialize() {
        Local<FunctionTemplate> t = FunctionTemplate::New();

//        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "getName",
                                      NetworkFilter::GetName);
        NODE_SET_PROTOTYPE_METHOD(t, "getUUID",
                                      NetworkFilter::GetUUID);
        NODE_SET_PROTOTYPE_METHOD(t, "undefine",
                                      NetworkFilter::Undefine);
        NODE_SET_PROTOTYPE_METHOD(t, "toXml",
                                      NetworkFilter::ToXml);

        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->SetClassName(String::NewSymbol("NetworkFilter"));
    }

    Handle<Value> NetworkFilter::LookupByName(const Arguments& args) {
        HandleScope scope;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid Network Filter name.")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value name(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        NetworkFilter *nwfilter = new NetworkFilter();
        nwfilter->filter_ = virNWFilterLookupByName(hypervisor->connection(), (const char *) *name);

        if(nwfilter->filter_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> nwfilter_obj = nwfilter->constructor_template->GetFunction()->NewInstance();

        nwfilter->Wrap(nwfilter_obj);

        return scope.Close(nwfilter_obj);
    }

    Handle<Value> NetworkFilter::LookupByUUID(const Arguments& args) {
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

        NetworkFilter *nwfilter = new NetworkFilter();
        nwfilter->filter_ = virNWFilterLookupByUUIDString(hypervisor->connection(), (const char *) *uuid);

        if(nwfilter->filter_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> nwfilter_obj = nwfilter->constructor_template->GetFunction()->NewInstance();

        nwfilter->Wrap(nwfilter_obj);

        return scope.Close(nwfilter_obj);
    }

    Handle<Value> NetworkFilter::Define(const Arguments& args) {
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

        String::Utf8Value xml(args[0]->ToString());

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        NetworkFilter *nwfilter = new NetworkFilter();
        nwfilter->filter_ = virNWFilterDefineXML(hypervisor->connection(), (const char *) *xml);

        if(nwfilter->filter_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> nwfilter_obj = nwfilter->constructor_template->GetFunction()->NewInstance();

        nwfilter->Wrap(nwfilter_obj);

        return scope.Close(nwfilter_obj);
    }

    Handle<Value> NetworkFilter::GetName(const Arguments& args) {
        HandleScope scope;
        const char *name = NULL;

        NetworkFilter *nwfilter = ObjectWrap::Unwrap<NetworkFilter>(args.This());

        name = virNWFilterGetName(nwfilter->filter_);

        if(name == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(String::New(name));
    }

    Handle<Value> NetworkFilter::GetUUID(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        char *uuid = new char[VIR_UUID_STRING_BUFLEN];

        NetworkFilter *nwfilter = ObjectWrap::Unwrap<NetworkFilter>(args.This());

        ret = virNWFilterGetUUIDString(nwfilter->filter_, uuid);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            delete[] uuid;
            return Null();
        }

        Local<String> uuid_str = String::New(uuid);

        delete[] uuid;

        return scope.Close(uuid_str);
    }

    Handle<Value> NetworkFilter::Undefine(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        NetworkFilter *nwfilter = ObjectWrap::Unwrap<NetworkFilter>(args.This());
        ret = virNWFilterUndefine(nwfilter->filter_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> NetworkFilter::ToXml(const Arguments& args) {
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

        NetworkFilter *nwfilter = ObjectWrap::Unwrap<NetworkFilter>(args.This());
        xml_ = virNWFilterGetXMLDesc(nwfilter->filter_, flags);

        if(xml_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> xml = String::New(xml_);

        free(xml_);

        return scope.Close(xml);
    }
}

