// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_DOMAIN_SNAPSHOT_H_
#define SRC_DOMAIN_SNAPSHOT_H_

#include "node_libvirt.h"
#include "domain_snapshot.h"
#include "error.h"

namespace NodeLibvirt {

    class DomainSnapshot : public EventEmitter {
        friend class Domain;

        protected:
            static Handle<Value> HasCurrentSnapshot(const Arguments& args);
            static Handle<Value> RevertToSnapshot(const Arguments& args);
            static Handle<Value> CreateSnapshotXml(const Arguments& args);
            static Handle<Value> GetCurrentSnapshot(const Arguments& args);
            static Handle<Value> DeleteSnapshot(const Arguments& args);
            static Handle<Value> FreeSnapshot(const Arguments& args);// maybe yes maybe not ?
            static Handle<Value> GetSnapshotXml(const Arguments& args);
            static Handle<Value> ListSnapshotsNames(const Arguments& args);
            static Handle<Value> LookupSnapshotByName(const Arguments& args);
            static Handle<Value> GetSnapshotsCount(const Arguments& args);
    };

}  //namespace NodeLibvirt

#endif  // SRC_DOMAIN_SNAPSHOT_H_

