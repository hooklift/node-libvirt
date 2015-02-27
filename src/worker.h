#ifndef SRC_WORKER_H_
#define SRC_WORKER_H_

#include <node.h>
#include <nan.h>
#include <libvirt/virterror.h>

namespace NodeLibvirt {

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

}

#endif // SRC_WORKER_H_
