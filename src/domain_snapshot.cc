// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <node_buffer.h>
#include "domain.h"

namespace NodeLibvirt {

    Handle<Value> DomainSnapshot::HasCurrentSnapshot(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        int ret = -1;

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());

        ret = virDomainHasCurrentSnapshot(domain->domain_, flags);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return ret == 1 ? True() : False();
    }

    Handle<Value> DomainSnapshot::RevertToSnapshot(const Arguments& args) {
        HandleScope scope;
        virDomainSnapshotPtr snapshot;
        const char* name = NULL;
        unsigned int flags = 0;
        int ret = -1;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string as argument to invoke this function")));
        }
        String::Utf8Value name_(args[0]->ToString());
        name = ToCString(name_);

        Domain *domain = ObjectWrap::Unwrap<Domain>(args.This());
        snapshot = virDomainSnapshotLookupByName(domain->domain_, name, flags);
        if(snapshot == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        ret = virDomainRevertToSnapshot(snapshot, flags);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> DomainSnapshot::CreateSnapshotXml(const Arguments& args) {
        HandleScope scope;
    }

    Handle<Value> DomainSnapshot::GetCurrentSnapshot(const Arguments& args) {
        HandleScope scope;
    }

    Handle<Value> DomainSnapshot::DeleteSnapshot(const Arguments& args) {
        HandleScope scope;
    }

    Handle<Value> DomainSnapshot::FreeSnapshot(const Arguments& args) {
        HandleScope scope;
    }

    Handle<Value> DomainSnapshot::GetSnapshotXml(const Arguments& args) {
        HandleScope scope;
    }

    Handle<Value> DomainSnapshot::ListSnapshotsNames(const Arguments& args) {
        HandleScope scope;
    }

    Handle<Value> DomainSnapshot::GetSnapshotsCount(const Arguments& args) {
        HandleScope scope;
    }
} //namespace NodeLibvirt

