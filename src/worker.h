#ifndef SRC_WORKER_H_
#define SRC_WORKER_H_

#include <node.h>
#include <nan.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#define NOARGS_WORKER_METHOD(name, worker)                                  \
                                                                            \
NAN_METHOD(name) {                                                          \
    NanScope();                                                             \
                                                                            \
    Hypervisor *hypervisor = ObjectWrap::Unwrap<Hypervisor>(args.This());   \
                                                                            \
    if (args.Length() == 1 && !args[0]->IsFunction()) {                     \
        return ThrowException(Exception::TypeError(                         \
        String::New("You must specify a function as first argument")));     \
    }                                                                       \
                                                                            \
    NanCallback *callback = new NanCallback(args[0].As<Function>());        \
                                                                            \
    NanAsyncQueueWorker(new worker(callback, hypervisor));                  \
                                                                            \
    NanReturnUndefined();                                                   \
}

namespace NodeLibvirt {

    typedef int (*GetNumOfType)(virConnectPtr);
    typedef int (*GetListOfType)(virConnectPtr, char**, int);

    class Hypervisor;

    class LibvirtWorker : public NanAsyncWorker {
        public:
            LibvirtWorker(NanCallback *callback, Hypervisor *hypervisor);

            Hypervisor* getHypervisor();
            void setVirError(virErrorPtr error);
            virErrorPtr getVirError();
            virtual void WorkComplete();
        protected:
            virtual void HandleErrorCallback ();
        private:
            Hypervisor *hypervisor_;
            virErrorPtr virerror_;
    };

    class StringWorker : public LibvirtWorker {
        public:
            StringWorker(NanCallback *callback, Hypervisor *hypervisor);

            void setString(char *str);
            char* getString();
        protected:
            void HandleOKCallback();
        private:
            char *str_;
    };

    class GetListOfWorker : public LibvirtWorker {
        public:
            GetListOfWorker(NanCallback *callback,
                Hypervisor *hypervisor,
                GetNumOfType numof_function,
                GetListOfType listof_function
            );
            void Execute();

        protected:
            void HandleOKCallback();
        private:
            GetNumOfType numof_function_;
            GetListOfType listof_function_;
            char **names_;
            int size_;
    };

    class GetNumOfWorker : public LibvirtWorker {
        public:
            GetNumOfWorker(NanCallback *callback,
                Hypervisor *hypervisor,
                GetNumOfType numof_function
            );
            void Execute();

        protected:
            void HandleOKCallback();
        private:
            GetNumOfType numof_function_;
            int size_;
    };

}

#endif // SRC_WORKER_H_
