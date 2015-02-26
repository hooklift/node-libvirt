#ifndef SRC_BATON_H_
#define SRC_BATON_H_

#include <node.h>
#include <libvirt/virterror.h>
#include "hypervisor.h"

namespace NodeLibvirt {
    class Baton {
        public:
            Baton(Hypervisor *hypervisor, Persistent<Function> callback);
            Persistent<Function> getCallback();
            Hypervisor* getHypervisor();
            uv_work_t* getHandle();
            void setError(virErrorPtr error);
            virErrorPtr getError();
            bool hasError();

        private:
            uv_work_t request_;
            Persistent<Function> callback_;
            Hypervisor *hypervisor_;
            virErrorPtr error_;
    };

    class ConnectBaton : public Baton {
        public:
            ConnectBaton(Hypervisor *hypervisor, Persistent<Function> callback);
    };

    class StringBaton : public Baton {
        public:
            StringBaton(Hypervisor *hypervisor, Persistent<Function> callback);
            void setString(char *);
            char* getString();
        private:
            char *str_;
    };
}

#endif // SRC_BATON_H_
