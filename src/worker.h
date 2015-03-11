#ifndef SRC_WORKER_H_
#define SRC_WORKER_H_

#include <node.h>
#include <nan.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

namespace NodeLibvirt {

    typedef int (*GetNumOfType)(virConnectPtr);
    typedef int (*GetListOfType)(virConnectPtr, char**, int);

    class Hypervisor;

    class LibvirtWorker : public NanAsyncWorker {
        public:
            LibvirtWorker(NanCallback *callback, virConnectPtr conn);

            virConnectPtr getConnection();
            void setVirError(virErrorPtr error);
            virErrorPtr getVirError();
            virtual void WorkComplete();
        protected:
            virtual void HandleErrorCallback ();
        private:
            virConnectPtr conn_;
            virErrorPtr virerror_;
    };

    template <class T, class U, class V, class W>
    class HelperWorker : public T {
        public:
            HelperWorker(NanCallback *callback, U conn);
            HelperWorker(NanCallback *callback, U conn, V val);

            virtual void setVal(V val);
            virtual V getVal();
        protected:
            void HandleOKCallback();
            V val_;
    };

    template <class T, class U, class V, class W>
    HelperWorker<T, U, V, W>::HelperWorker(NanCallback *callback, U obj)
    : T(callback, obj) {
    }

    template <class T, class U, class V, class W>
    HelperWorker<T, U, V, W>::HelperWorker(NanCallback *callback, U obj, V val)
    : T(callback, obj), val_(val) {
    }

    template <class T, class U, class V, class W>
    void HelperWorker<T, U, V, W>::setVal(V val) {
        val_ = val;
    }

    template <class T, class U, class V, class W>
    V HelperWorker<T, U, V, W>::getVal() {
        return val_;
    }

    template <class T, class U, class V, class W>
    void HelperWorker<T, U, V, W>::HandleOKCallback() {
        NanScope();

        v8::Local<v8::Value> argv[] = { NanNull(), NanNew<W>(val_) };

        this->callback->Call(2, argv);
    }

    template <class T, class U>
    class StringReturnWorker : public HelperWorker<T, U, char*, v8::String> {
        public:
            StringReturnWorker(NanCallback *callback, U obj, bool freeit = true);
            ~StringReturnWorker();
        private:
            bool freeit_;
    };

    template <class T, class U>
    StringReturnWorker<T, U>::StringReturnWorker(NanCallback *callback, U obj, bool freeit)
    : HelperWorker<T, U, char*, v8::String>(callback, obj, NULL), freeit_(freeit) {
    }

    template <class T, class U>
    StringReturnWorker<T, U>::~StringReturnWorker() {
        if (this->getVal() != NULL && freeit_)
            delete this->getVal();
    }

    template <class T, class U>
    class BooleanReturnWorker : public HelperWorker<T, U, bool, v8::Boolean> {
        public:
            BooleanReturnWorker(NanCallback *callback, U obj);
    };

    template <class T, class U>
    BooleanReturnWorker<T, U>::BooleanReturnWorker(NanCallback *callback, U obj)
    : HelperWorker<T, U, bool, v8::Boolean>(callback, obj, false) {
    }

    class GetListOfWorker : public LibvirtWorker {
        public:
            GetListOfWorker(NanCallback *callback,
                virConnectPtr conn,
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
                virConnectPtr conn,
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
