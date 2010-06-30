// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <stdlib.h>
#include "node_device.h"

namespace NodeLibvirt {
    Persistent<FunctionTemplate> NodeDevice::constructor_template;

    void NodeDevice::Initialize() {
        Local<FunctionTemplate> t = FunctionTemplate::New();

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "destroy",
                                      NodeDevice::Destroy);

        NODE_SET_PROTOTYPE_METHOD(t, "detach",
                                      NodeDevice::Detach);

        NODE_SET_PROTOTYPE_METHOD(t, "reattach",
                                      NodeDevice::Reattach);

        NODE_SET_PROTOTYPE_METHOD(t, "reset",
                                      NodeDevice::Destroy);

        NODE_SET_PROTOTYPE_METHOD(t, "getName",
                                      NodeDevice::GetName);

        NODE_SET_PROTOTYPE_METHOD(t, "getParentName",
                                      NodeDevice::GetParentName);

        NODE_SET_PROTOTYPE_METHOD(t, "toXml",
                                      NodeDevice::ToXml);

        NODE_SET_PROTOTYPE_METHOD(t, "getCapabilities",
                                      NodeDevice::GetCapabilities);

        constructor_template = Persistent<FunctionTemplate>::New(t);
        constructor_template->SetClassName(String::NewSymbol("NodeDevice"));

        Local<ObjectTemplate> object_tmpl = t->InstanceTemplate();
        //Constants initialization
    }

    Handle<Value> NodeDevice::LookupByName(const Arguments& args) {
        HandleScope scope;
        const char* name = NULL;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string to call this function")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value name_(args[0]->ToString());

        name = ToCString(name_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        NodeDevice *device = new NodeDevice();
        device->device_ = virNodeDeviceLookupByName(hypervisor->connection(), name);

        if(device->device_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> device_obj = device->constructor_template->GetFunction()->NewInstance();

        device->Wrap(device_obj);

        return scope.Close(device_obj);
    }

    Handle<Value> NodeDevice::Create(const Arguments& args) {
        HandleScope scope;
        const char *xml = NULL;
        unsigned int flags = 0;

        if(args.Length() == 0 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a string to call this function")));
        }

        Local<Object> hyp_obj = args.This();

        if(!Hypervisor::HasInstance(hyp_obj)) {
            return ThrowException(Exception::TypeError(
            String::New("You must specify a Hypervisor instance")));
        }

        String::Utf8Value xml_(args[0]->ToString());
        xml = ToCString(xml_);

        Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(hyp_obj);

        NodeDevice *device = new NodeDevice();
        device->device_ = virNodeDeviceCreateXML(hypervisor->connection(), xml, flags);

        if(device->device_ == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Object> device_obj = device->constructor_template->GetFunction()->NewInstance();

        device->Wrap(device_obj);

        return scope.Close(device_obj);
    }

    //Really neccesary call destroy from javascript ???
    Handle<Value> NodeDevice::Destroy(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());

        ret = virNodeDeviceDestroy(device->device_);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> NodeDevice::Detach(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
        ret = virNodeDeviceDettach(device->device_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> NodeDevice::Reattach(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
        ret = virNodeDeviceReAttach(device->device_);

        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> NodeDevice::Reset(const Arguments& args) {
        HandleScope scope;
        int ret = -1;

        NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());

        ret = virNodeDeviceReset(device->device_);
        if(ret == -1) {
            ThrowException(Error::New(virGetLastError()));
            return False();
        }

        return True();
    }

    Handle<Value> NodeDevice::GetName(const Arguments& args) {
        HandleScope scope;
        const char *name = NULL;

        NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
        name = virNodeDeviceGetName(device->device_);

        if(name == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return String::New(name);
    }

    Handle<Value> NodeDevice::GetParentName(const Arguments& args) {
        HandleScope scope;
        const char *name = NULL;

        NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
        name = virNodeDeviceGetParent(device->device_);

        if(name == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return String::New(name);
    }

    Handle<Value> NodeDevice::ToXml(const Arguments& args) {
        HandleScope scope;
        unsigned int flags = 0;
        const char *xml = NULL;

        NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());
        xml = virNodeDeviceGetXMLDesc(device->device_, flags);

        if(xml == NULL) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        return String::New(xml);
    }

    Handle<Value> NodeDevice::GetCapabilities(const Arguments& args) {
        HandleScope scope;
        const char *capabilities = NULL;
        char **names_ = NULL;
        int numcaps = -1;

        NodeDevice *device = ObjectWrap::Unwrap<NodeDevice>(args.This());

        numcaps = virNodeDeviceNumOfCaps(device->device_);
        if(numcaps == -1) {
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        names_ = (char**) malloc(sizeof(*names_) * numcaps);
        if(names_ == NULL) {
            LIBVIRT_THROW_EXCEPTION("Unable to allocate memory");
            return Null();
        }

        numcaps = virNodeDeviceListCaps(device->device_, names_, numcaps);
        if(numcaps == -1) {
            free(names_);
            ThrowException(Error::New(virGetLastError()));
            return Null();
        }

        Local<Array> names = Array::New(numcaps);

        for(int i = 0; i < numcaps; i++) {
            names->Set(Integer::New(i), String::New(names_[i]));
            free(names_[i]);
        }
        free(names_);

        return names;
    }

} //namespace NodeLibvirt

