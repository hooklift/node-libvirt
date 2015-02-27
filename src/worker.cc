#include "worker.h"
#include "hypervisor.h"
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

        if (virerror_ == NULL && ErrorMessage() == NULL)
            HandleOKCallback();
        else
            HandleErrorCallback();

        delete callback;
        callback = NULL;
    }

    void LibvirtWorker::HandleErrorCallback() {
        NanScope();

        if (virerror_ != NULL) {
            v8::Handle<v8::Value> argv[] = { Error::New(getVirError()) };
            callback->Call(1, argv);
        } else {
            v8::Local<v8::Value> argv[] = {
                v8::Exception::Error(NanNew<v8::String>(ErrorMessage()))
            };
            callback->Call(1, argv);
        }
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

    GetListOfWorker::GetListOfWorker(
        NanCallback *callback,
        Hypervisor *hypervisor,
        GetNumOfType numof_function,
        GetListOfType listof_function)
    : LibvirtWorker(callback, hypervisor), numof_function_(numof_function), listof_function_(listof_function), names_(NULL), size_(0) {
    }

    void GetListOfWorker::Execute() {
        virConnectPtr connection;

        Hypervisor *hypervisor = getHypervisor();
        connection = hypervisor->connection();

        size_ = numof_function_(connection);

        if(size_ == -1) {
            setVirError(virGetLastError());
            return;
        }

        names_ = (char **)malloc(sizeof(*names_) * size_);
        if(names_ == NULL) {
            SetErrorMessage("unable to allocate memory");
            return;
        }

        int ret = listof_function_(connection, names_, size_);

        if(ret == -1) {
            setVirError(virGetLastError());
        }

    }

    void GetListOfWorker::HandleOKCallback() {
        NanScope();

        v8::Local<Array> v8Array = NanNew<v8::Array>(size_);
        for(int i = 0; i < size_; i++) {
            v8Array->Set(NanNew<v8::Integer>(i), NanNew<v8::String>(names_[i]));
            free(names_[i]);
        }

        free(names_);

        v8::Local<v8::Value> argv[] = { NanNull(), v8Array };

        callback->Call(2, argv);
    }
}
