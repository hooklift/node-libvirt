// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_CONNECT_H_
#define SRC_CONNECT_H_

#include "node_libvirt.h"

namespace NodeLibvirt {

    class Connection : public EventEmitter {
        public:
            static void Initialize(v8::Handle<v8::Object> target);
    };

}  // namespace NodeLibvirt

#endif  // SRC_CONNECT_H_
