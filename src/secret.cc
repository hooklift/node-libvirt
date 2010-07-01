// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include <string.h>
#include "secret.h"

namespace NodeLibvirt {
    Persistent<FunctionTemplate> Secret::constructor_template;

    void Secret::Initialize() {
        Local<FunctionTemplate> t = FunctionTemplate::New();

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "undefine",
                                      Secret::Undefine);
        NODE_SET_PROTOTYPE_METHOD(t, "getUUID",
                                      Secret::GetUUID);
        NODE_SET_PROTOTYPE_METHOD(t, "getValue",
                                      Secret::GetValue);
        NODE_SET_PROTOTYPE_METHOD(t, "setValue",
                                      Secret::SetValue);
        NODE_SET_PROTOTYPE_METHOD(t, "getUsageId",
                                      Secret::GetUsageId);
        NODE_SET_PROTOTYPE_METHOD(t, "getUsageType",
                                      Secret::GetUsageType);
        NODE_SET_PROTOTYPE_METHOD(t, "toXml",
                                      Secret::ToXml);

        Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();

        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->SetClassName(String::NewSymbol("Secret"));
    }

    Handle<Value> Secret::Define(const Arguments& args) {
        HandleScope scope;
        const char* xml = NULL;
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

        String::Utf8Value xml_(args[0]->ToString());
        xml = ToCString(xml_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Secret *secret = new Secret();
        secret->secret_ = virSecretDefineXML(hypervisor->connection(), xml, flags);

        if(secret->secret_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> secret_obj = secret->constructor_template->GetFunction()->NewInstance();

        secret->Wrap(secret_obj);

        return scope.Close(secret_obj);
    }

    Handle<Value> Secret::Undefine(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
        ret = virSecretUndefine(secret->secret_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        if(secret->secret_ != NULL) {
            virSecretFree(secret->secret_);
        }

        return True();
    }

    Handle<Value> Secret::LookupByUsage(const Arguments& args) {
        HandleScope scope;
        const char* usage_id = NULL;
        int usage_type = VIR_SECRET_USAGE_TYPE_NONE;

        if(args.Length() < 2) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify two arguments to call this function")));
        }

        if(!args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a number as first argument")));
        }

        if(!args[1]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as second argument")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        usage_type = args[0]->Int32Value();

        String::Utf8Value usage_id_(args[1]->ToString());
        usage_id = ToCString(usage_id_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        Secret *secret = new Secret();
        secret->secret_ = virSecretLookupByUsage(hypervisor->connection(), usage_type, usage_id);

        if(secret->secret_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> secret_obj = secret->constructor_template->GetFunction()->NewInstance();

        secret->Wrap(secret_obj);

        return scope.Close(secret_obj);
    }

    Handle<Value> Secret::LookupByUUID(const Arguments& args) {
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

        Secret *secret = new Secret();
        secret->secret_ = virSecretLookupByUUIDString(hypervisor->connection(), uuid);

        if(secret->secret_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> secret_obj = secret->constructor_template->GetFunction()->NewInstance();

        secret->Wrap(secret_obj);

        return scope.Close(secret_obj);
    }

    Handle<Value> Secret::GetUUID(const Arguments& args) {
        HandleScope scope;
        int ret = -1;
        char *uuid = new char[VIR_UUID_STRING_BUFLEN];

        Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());

        ret = virSecretGetUUIDString(secret->secret_, uuid);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            delete[] uuid;
            return Null();
        }

        Local<String> uuid_str = String::New(uuid);

        delete[] uuid;

        return scope.Close(uuid_str);
    }

    Handle<Value> Secret::GetUsageId(const Arguments& args) {
        HandleScope scope;
        const char *usage_id = NULL;

        Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());

        usage_id = virSecretGetUsageID(secret->secret_);

        if(usage_id == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(String::New(usage_id));
    }

    Handle<Value> Secret::GetUsageType(const Arguments& args) {
        HandleScope scope;
        int usage_type = VIR_SECRET_USAGE_TYPE_NONE;

        Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());

        usage_type = virSecretGetUsageType(secret->secret_);

        if(usage_type == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return scope.Close(Integer::New(usage_type));
    }

    Handle<Value> Secret::GetValue(const Arguments& args) {
        HandleScope scope;
        size_t size;
        unsigned int flags = 0;
        unsigned char* value_ = NULL;

        Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());

        value_ = virSecretGetValue(secret->secret_, &size, flags);

        if(value_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }
        Local<String> value = String::New((const char *)value_, size);
        memset(value_, 0, size);
        free(value_);

        return scope.Close(value);
    }

    Handle<Value> Secret::SetValue(const Arguments& args) {
        HandleScope scope;
        const char *value = NULL;
        int size = 0;
        unsigned int flags = 0;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a valid secret value")));
        }

        String::Utf8Value value_(args[0]->ToString());

        value = ToCString(value_);

        Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());

        ret = virSecretSetValue(secret->secret_, (const unsigned char *)value, sizeof(*value), flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> Secret::ToXml(const Arguments& args) {
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

        Secret *secret = ObjectWrap::Unwrap<Secret>(args.This());
        xml_ = virSecretGetXMLDesc(secret->secret_, flags);

        if(xml_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<String> xml = String::New(xml_);

        free(xml_);

        return scope.Close(xml);
    }
}

