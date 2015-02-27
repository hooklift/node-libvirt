#include "worker.h"
#include "error.h"

namespace NodeLibvirt {

    LibvirtWorker::LibvirtWorker(NanCallback *callback, Hypervisor *hypervisor)
    : NanAsyncWorker(callback), hypervisor_(hypervisor), virerror_(NULL) {
    }

    Hypervisor* LibvirtWorker::getHypervisor() {
        return hypervisor_;
    }

    void LibvirtWorker::setVirError(virErrorPtr error) {
        virerror_ = error;
    }

    virErrorPtr LibvirtWorker::getVirError() {
        return virerror_;
    }

    void LibvirtWorker::WorkComplete() {
        NanScope();

        if (virerror_ == NULL)
            HandleOKCallback();
        else
            HandleErrorCallback();

        delete callback;
        callback = NULL;
    }

    void LibvirtWorker::HandleErrorCallback() {
        NanScope();

        v8::Handle<v8::Value> argv[] = { Error::New(getVirError()) };

        callback->Call(1, argv);
    }

    StringWorker::StringWorker(NanCallback *callback, Hypervisor *hypervisor)
    : LibvirtWorker(callback, hypervisor), str_(NULL) {
    }

    void StringWorker::setString(char *str) {
        str_ = str;
    }

    char* StringWorker::getString() {
        return str_;
    }

    void StringWorker::HandleOKCallback() {
        NanScope();

        v8::Local<v8::Value> argv[] = { NanNull(), NanNew<v8::String>(str_) };
        free(str_);

        callback->Call(2, argv);
    }
}
