// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_ERROR_H_
#define SRC_ERROR_H_

#include "node_libvirt.h"

namespace NodeLibvirt {

    class Error : ObjectWrap {
        public:
            static void Initialize();
            static Handle<Value> New(virErrorPtr error);
        private:
            virErrorPtr error_;
            static Persistent<FunctionTemplate> constructor_template;
            static Handle<Value> Getter(Local<String> property,
                                        const AccessorInfo& info);

            Error(virErrorPtr error);
    };

} //namespace NodeLibvirt

#endif // SRC_ERROR_H_

