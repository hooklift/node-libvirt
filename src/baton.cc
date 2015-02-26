#include "baton.h"

namespace NodeLibvirt {

    Baton::Baton(Hypervisor *hypervisor, Persistent<Function> callback) {
        this->hypervisor_ = hypervisor;
        this->callback_ = callback;
        this->request_.data = this;
        this->error_ = NULL;
    }

    Persistent<Function> Baton::getCallback() {
        return this->callback_;
    }

    Hypervisor* Baton::getHypervisor() {
        return this->hypervisor_;
    }

    uv_work_t* Baton::getHandle() {
        return &this->request_;
    }

    void Baton::setError(virErrorPtr error) {
      this->error_ = error;
    }

    virErrorPtr Baton::getError() {
      return this->error_;
    }

    bool Baton::hasError() {
      return this->error_ != NULL;
    }

    ConnectBaton::ConnectBaton(Hypervisor *hypervisor, Persistent<Function> callback)
    : Baton(hypervisor, callback) {
    }

}
