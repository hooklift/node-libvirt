// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_EVENTIMPL_H_
#define SRC_EVENTIMPL_H_

#include "node_libvirt.h"
#include "error.h"
#include <vector>

namespace NodeLibvirt {

    struct nodeEventHandle {
        int watch;
        virEventHandleCallback cb;
        void *opaque;
        int event;
        int running;
        int deleted;
        int fd;
        uv_poll_t watcher;
    };

    struct nodeEventTimeout {
        int timer;
        int frequency;
        virEventTimeoutCallback cb;
        void *opaque;
        int deleted;
        uv_timer_t timerWatcher;
        uv_check_t checkWatcher;
    };

    class EventImpl {
        public:
            static void Initialize(Handle<Object> target);
            static Handle<Value> SetupEvent(const Arguments& args);

            static int AddHandle(int fd, int event, virEventHandleCallback cb, void *opaque, virFreeCallback ff);
            static void UpdateHandle(int watch, int event);
            static int RemoveHandle(int watch);
            static int AddTimeout(int frequency, virEventTimeoutCallback cb, void *opaque, virFreeCallback ff);
            static void UpdateTimeout(int timer, int timeout);
            static int RemoveTimeout(int timer);

            static void HandleCallback(uv_poll_t* handle, int status, int events);
            static void CheckCallback(uv_check_t* handle, int status);
            static void TimerCallback(uv_timer_t* handle, int status);
            static void TimeoutCallback(uv_handle_s* handle);

            static int EventToUV(int);
            static int EventFromUV(int);

            static nodeEventHandle* FindHandle(int watch);
            static nodeEventHandle* FindDeletedHandle();
            static nodeEventTimeout* FindTimeout(int watch);
            static nodeEventTimeout* FindDeletedTimeout();

            static int nextWatch;
            static int nextTimeout;
            static std::vector<nodeEventHandle*> handles;
            static std::vector<nodeEventTimeout*> timeouts;
        private:
    };

}  //namespace NodeLibvirt

#endif  // SRC_EVENTIMPL_H

