// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <cstdio>
#include <cstdlib>
#include "event_impl.h"

namespace NodeLibvirt {

    int EventImpl::nextWatch = 1;
    int EventImpl::nextTimeout = 1;
    std::vector<nodeEventHandle*> EventImpl::handles;
    std::vector<nodeEventTimeout*> EventImpl::timeouts;

    void EventImpl::Initialize(Handle<Object> target) {
        target->Set(String::NewSymbol("setupEvent"), FunctionTemplate::New(SetupEvent)->GetFunction());
    }

    Handle<Value> EventImpl::SetupEvent(const Arguments& args) {
        HandleScope scope;

        virEventRegisterImpl(
            AddHandle,
            UpdateHandle,
            RemoveHandle,
            AddTimeout,
            UpdateTimeout,
            RemoveTimeout
            );

        return scope.Close(Undefined());
    }

    void EventImpl::HandleCallback(uv_poll_t* handle, int status, int events) {
        nodeEventHandle *h = (nodeEventHandle*) handle->data;
        virEventHandleCallback cb = h->cb;

        (cb)(h->watch, h->fd, EventImpl::EventFromUV(events), h->opaque);
    }

    void EventImpl::CheckCallback(uv_check_t* handle, int status) {
        TimeoutCallback((uv_handle_s*)handle);
    }

    void EventImpl::TimerCallback(uv_timer_t* handle, int status) {
        TimeoutCallback((uv_handle_s*)handle);
    }

    void EventImpl::TimeoutCallback(uv_handle_s* handle) {
        nodeEventTimeout *t = (nodeEventTimeout*) handle->data;
        virEventTimeoutCallback cb = t->cb;

        (cb)(t->timer, t->opaque);
    }

    int EventImpl::AddHandle(int fd, int event, virEventHandleCallback cb, void *opaque, virFreeCallback ff) {
        nodeEventHandle *handle;

        handle = FindDeletedHandle();
        if (handle == NULL) {
            handle = new nodeEventHandle();
            handle->watch = nextWatch++;
            handles.push_back(handle);
        }

        handle->cb = cb;
        handle->fd = fd;
        handle->event = event;
        handle->running = 0;
        handle->deleted = 0;
        handle->opaque = opaque;

        uv_poll_init(uv_default_loop(), &handle->watcher, handle->fd);
        handle->watcher.data = handle;

        if (event) {
            uv_poll_start(&handle->watcher, EventToUV(event), HandleCallback);
            handle->running = 1;
        }

        return handle->watch;
    }

    void EventImpl::UpdateHandle(int watch, int event) {
        nodeEventHandle* handle;
        handle = FindHandle(watch);

        if (handle == NULL)
            return;

        if (!event && handle->running) {
            //fprintf(stderr, " STOP POLL, watch=%d event=%d\n", watch, event);
	    uv_poll_stop(&handle->watcher);
            handle->running = 0;
        } else if (event && handle->event == event && !handle->running) {
            //fprintf(stderr, " RESTART POLL, watch=%d event=%d\n", watch, event);
	    uv_poll_start(&handle->watcher, EventToUV(event), HandleCallback);
            handle->running = 1;
        } else if (event && handle->event != event) {
            //fprintf(stderr, " MODIFYING POLL, watch=%d event=%d\n", watch, event);
            uv_poll_start(&handle->watcher, EventToUV(event), HandleCallback);
            handle->event = event;
            handle->running = 1;
        }
    }

    int EventImpl::RemoveHandle(int watch) {
        nodeEventHandle* handle;
        handle = FindHandle(watch);

        if (handle == NULL)
            return -1;

        uv_poll_stop(&handle->watcher);
        handle->deleted = 1;

        return 0;
    }

    nodeEventHandle* EventImpl::FindHandle(int watch) {
        for (std::vector<nodeEventHandle*>::iterator it = handles.begin() ; it != handles.end(); ++it) {
            if ((*it)->watch == watch)
                return *it;
        }
        return NULL;
    }

    nodeEventHandle* EventImpl::FindDeletedHandle() {
        for (std::vector<nodeEventHandle*>::iterator it = handles.begin() ; it != handles.end(); ++it) {
            if ((*it)->deleted)
                return *it;
        }
        return NULL;
    }

    int EventImpl::AddTimeout(int frequency, virEventTimeoutCallback cb, void *opaque, virFreeCallback ff) {
        //fprintf(stderr, "Adding timeout, freq=%d\n", frequency);
        nodeEventTimeout *timeout;

        timeout = FindDeletedTimeout();
        if (timeout == NULL) {
            timeout = new nodeEventTimeout();
            timeout->timer = nextTimeout++;
            timeouts.push_back(timeout);
        }

        timeout->cb = cb;
        timeout->frequency = frequency;
        timeout->deleted = 0; 
        timeout->opaque = opaque;

        uv_check_init(uv_default_loop(), &timeout->checkWatcher);
        uv_timer_init(uv_default_loop(), &timeout->timerWatcher);
        
        timeout->checkWatcher.data = timeout;
        timeout->timerWatcher.data = timeout;

        if (frequency == 0) {
            uv_check_start(&timeout->checkWatcher, CheckCallback);
        } else if (frequency >= 0) {
            uv_timer_start(&timeout->timerWatcher, TimerCallback, frequency, frequency);
        }

        return timeout->timer;
    }

    void EventImpl::UpdateTimeout(int timer, int frequency) {
        //fprintf(stderr, "update timeout, timer=%d timeout=%d\n", timer, frequency);
        nodeEventTimeout* timeout = FindTimeout(timer);

        if (timeout == NULL)
            return;

        if (frequency == timeout->frequency)
            return;

        uv_timer_stop(&timeout->timerWatcher);
        uv_check_stop(&timeout->checkWatcher);

        timeout->frequency = frequency;
        
        if (frequency == 0) {
            uv_check_start(&timeout->checkWatcher, CheckCallback);
        } else if (frequency >= 0) {
            uv_timer_start(&timeout->timerWatcher, TimerCallback, frequency, frequency);
        }
     
    }

    int EventImpl::RemoveTimeout(int timer) {
        //fprintf(stderr, "remove timeout, timer=%d\n", timer);
        nodeEventTimeout* timeout = FindTimeout(timer);

        if (timeout == NULL)
            return -1;

        uv_timer_stop(&timeout->timerWatcher);
        uv_check_stop(&timeout->checkWatcher);
        timeout->deleted = 1;

        return 0;
    }

    nodeEventTimeout* EventImpl::FindTimeout(int timer) {
        for (std::vector<nodeEventTimeout*>::iterator it = timeouts.begin() ; it != timeouts.end(); ++it) {
            if ((*it)->timer == timer)
                return *it;
        }
        return NULL;
    }

    nodeEventTimeout* EventImpl::FindDeletedTimeout() {
        for (std::vector<nodeEventTimeout*>::iterator it = timeouts.begin() ; it != timeouts.end(); ++it) {
            if ((*it)->deleted)
                return *it;
        }
        return NULL;
    }

    int EventImpl::EventToUV(int event) {
        int ret = 0;

        if (event & VIR_EVENT_HANDLE_READABLE)
            ret |= UV_READABLE;
        if (event & VIR_EVENT_HANDLE_WRITABLE)
            ret |= UV_WRITABLE;

        return ret;
    }

    int EventImpl::EventFromUV(int event) {
        int ret = 0;

        if (event & UV_READABLE)
            ret |= VIR_EVENT_HANDLE_READABLE;
        if (event & UV_WRITABLE)
            ret |= VIR_EVENT_HANDLE_WRITABLE;

        return ret;
    }

} //namespace NodeLibvirt

