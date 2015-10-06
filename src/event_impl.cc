// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#include <cstdio>
#include <cstdlib>
#include "event_impl.h"

namespace NLV {

class nodeEventHandle
{
public:
  int watch;
  virEventHandleCallback cb;
  void *opaque;
  int event;
  int newEvent;
  int toDelete;
  int deleted;
  int fd;
  uv_poll_t watcher;
};

class nodeEventTimeout
{
public:
  int timer;
  int frequency;
  int newFrequency;
  virEventTimeoutCallback cb;
  void *opaque;
  int toDelete;
  int deleted;
  uv_timer_t timerWatcher;
  uv_check_t checkWatcher;
};


uv_check_t updateHandleChecker;
uv_mutex_t lock;
int EventImpl::nextWatch = 1;
int EventImpl::nextTimeout = 1;
std::vector<nodeEventHandle*> EventImpl::handles;
std::vector<nodeEventTimeout*> EventImpl::timeouts;

void EventImpl::Initialize(Handle<Object> exports)
{
  Nan::HandleScope scope;
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(SetupEvent);
  exports->Set(Nan::New("setupEvent").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(EventImpl::SetupEvent)
{
  Nan::HandleScope scope;

  uv_mutex_init(&lock);
  uv_check_init(uv_default_loop(), &updateHandleChecker);
  uv_check_start(&updateHandleChecker, EventImpl::UpdateHandlesOnce);

  virEventRegisterImpl(
    AddHandle, UpdateHandle, RemoveHandle,
    AddTimeout, UpdateTimeout, RemoveTimeout
  );

  return;
}

#if UV_VERSION_MAJOR < 1
void EventImpl::UpdateHandlesOnce(uv_check_t* handle, int status)
#else
void EventImpl::UpdateHandlesOnce(uv_check_t* handle)
#endif
{
  uv_mutex_lock(&lock);

  for (std::vector<nodeEventHandle*>::iterator it = handles.begin() ; it != handles.end(); ++it) {
    nodeEventHandle *handle = *it;

    if (handle->deleted || (handle->newEvent == handle->event && !handle->toDelete))
      continue;

    if (handle->toDelete) {
      //fprintf(stderr, " CLOSE POLL, watch=%d event=%d\n", handle->watch, handle->newEvent);
      handle->toDelete = 0;
      uv_poll_stop(&handle->watcher);
      uv_close((uv_handle_t*)&handle->watcher, EventImpl::ClosePollCallback);
    } else if (EventToUV(handle->newEvent) == 0) {
      //fprintf(stderr, " STOP POLL, watch=%d event=%d\n", handle->watch, handle->newEvent);
      uv_poll_stop(&handle->watcher);
    } else {
      //fprintf(stderr, " START POLL, watch=%d event=%d\n", handle->watch, handle->newEvent);
      uv_poll_start(&handle->watcher, EventToUV(handle->newEvent), EventImpl::HandleCallback);
    }

    handle->event = handle->newEvent;
  }

  for (std::vector<nodeEventTimeout*>::iterator it = timeouts.begin() ; it != timeouts.end(); ++it) {
    nodeEventTimeout *timeout = *it;

    if (timeout->deleted || (timeout->newFrequency == timeout->frequency && !timeout->toDelete))
      continue;

    //fprintf(stderr, "CHANGE FREQ, freq=%d\n", timeout->newFrequency);
    uv_timer_stop(&timeout->timerWatcher);
    uv_check_stop(&timeout->checkWatcher);

    if (timeout->toDelete) {
      timeout->deleted = 1;
    } else if (timeout->newFrequency == 0) {
      uv_check_start(&timeout->checkWatcher, CheckCallback);
    } else if (timeout->newFrequency >= 0) {
      uv_timer_start(&timeout->timerWatcher, TimerCallback, timeout->newFrequency, timeout->newFrequency);
    }

    timeout->frequency = timeout->newFrequency;
  }

  uv_mutex_unlock(&lock);
}

void EventImpl::HandleCallback(uv_poll_t* handle, int status, int events)
{
  //fprintf(stderr, "dispatch handle=%d status=%d, events=%d\n", h->watch, status, events);

  nodeEventHandle *h = (nodeEventHandle*) handle->data;
  virEventHandleCallback cb = h->cb;

  (cb)(h->watch, h->fd, EventImpl::EventFromUV(events), h->opaque);
}

#if UV_VERSION_MAJOR < 1
void EventImpl::CheckCallback(uv_check_t* handle, int status)
#else
void EventImpl::CheckCallback(uv_check_t* handle)
#endif
{
  TimeoutCallback((uv_handle_s*)handle);
}

#if UV_VERSION_MAJOR < 1
void EventImpl::TimerCallback(uv_timer_t* handle, int status)
#else
void EventImpl::TimerCallback(uv_timer_t* handle)
#endif
{
  TimeoutCallback((uv_handle_s*)handle);
}

void EventImpl::TimeoutCallback(uv_handle_s* handle)
{
  //fprintf(stderr, "dispatch timer=%d\n", t->timer);

  nodeEventTimeout *t = (nodeEventTimeout*) handle->data;
  virEventTimeoutCallback cb = t->cb;

  (cb)(t->timer, t->opaque);

}

void EventImpl::ClosePollCallback(uv_handle_t* handle) {
  //fprintf(stderr, "close handle cb\n");

  uv_mutex_lock(&lock);

  nodeEventHandle *h = (nodeEventHandle*) handle->data;
  h->deleted = 1;

  uv_mutex_unlock(&lock);
}

int EventImpl::AddHandle(int fd, int event, virEventHandleCallback cb, void *opaque,
                         virFreeCallback ff)
{
  //fprintf(stderr, "Adding handle, fd=%d, event=%d\n", fd, event);

  uv_mutex_lock(&lock);
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
  handle->newEvent = event;
  handle->toDelete = 0;
  handle->deleted = 0;
  handle->opaque = opaque;

  uv_poll_init(uv_default_loop(), &handle->watcher, handle->fd);
  handle->watcher.data = handle;

  if (event) {
    uv_poll_start(&handle->watcher, EventToUV(event), HandleCallback);
  }

  uv_mutex_unlock(&lock);

  return handle->watch;
}

void EventImpl::UpdateHandle(int watch, int event)
{
  //fprintf(stderr, "Update handle, watch=%d, event=%d\n", watch, event);

  uv_mutex_lock(&lock);

  nodeEventHandle* handle;
  handle = FindHandle(watch);
  if (handle != NULL)
    handle->newEvent = event;

  uv_mutex_unlock(&lock);
}

int EventImpl::RemoveHandle(int watch)
{
  uv_mutex_lock(&lock);

  nodeEventHandle* handle;
  handle = FindHandle(watch);

  if (handle != NULL) {
    handle->newEvent = 0;
    handle->toDelete = 1;
  }

  uv_mutex_unlock(&lock);

  return 0;
}

nodeEventHandle* EventImpl::FindHandle(int watch)
{
  std::vector<nodeEventHandle*>::iterator it;
  for (it = handles.begin() ; it != handles.end(); ++it) {
    if ((*it)->watch == watch)
      return *it;
  }

  return NULL;
}

nodeEventHandle* EventImpl::FindDeletedHandle()
{
  std::vector<nodeEventHandle*>::iterator it;
  for (it = handles.begin() ; it != handles.end(); ++it) {
    if ((*it)->deleted)
      return *it;
  }

  return NULL;
}

int EventImpl::AddTimeout(int frequency, virEventTimeoutCallback cb, void *opaque,
                          virFreeCallback ff)
{
  //fprintf(stderr, "Adding timeout, freq=%d\n", frequency);

  uv_mutex_lock(&lock);

  nodeEventTimeout *timeout;

  timeout = FindDeletedTimeout();
  if (timeout == NULL) {
    timeout = new nodeEventTimeout();
    timeout->timer = nextTimeout++;

    uv_check_init(uv_default_loop(), &timeout->checkWatcher);
    uv_timer_init(uv_default_loop(), &timeout->timerWatcher);
    timeout->checkWatcher.data = timeout;
    timeout->timerWatcher.data = timeout;

    timeouts.push_back(timeout);
  }

  timeout->cb = cb;
  timeout->frequency = frequency;
  timeout->newFrequency = frequency;
  timeout->toDelete = 0;
  timeout->deleted = 0;
  timeout->opaque = opaque;

  timeout->checkWatcher.data = timeout;
  timeout->timerWatcher.data = timeout;

  if (frequency == 0) {
    uv_check_start(&timeout->checkWatcher, CheckCallback);
  } else if (frequency >= 0) {
    uv_timer_start(&timeout->timerWatcher, TimerCallback, frequency, frequency);
  }

  uv_mutex_unlock(&lock);

  return timeout->timer;
}

void EventImpl::UpdateTimeout(int timer, int frequency)
{
  //fprintf(stderr, "update timeout, timer=%d timeout=%d\n", timer, frequency);

  uv_mutex_lock(&lock);

  nodeEventTimeout* timeout = FindTimeout(timer);

  if (timeout != NULL)
    timeout->newFrequency = frequency;

  uv_mutex_unlock(&lock);
}

int EventImpl::RemoveTimeout(int timer)
{
  //fprintf(stderr, "remove timeout, timer=%d\n", timer);
  uv_mutex_lock(&lock);

  nodeEventTimeout* timeout = FindTimeout(timer);

  if (timeout != NULL)
    timeout->toDelete = 1;

  uv_mutex_unlock(&lock);

  return 0;
}

nodeEventTimeout* EventImpl::FindTimeout(int timer)
{
  std::vector<nodeEventTimeout*>::iterator it;
  for (it = timeouts.begin() ; it != timeouts.end(); ++it) {
    if ((*it)->timer == timer)
      return *it;
  }

  return NULL;
}

nodeEventTimeout* EventImpl::FindDeletedTimeout()
{
  std::vector<nodeEventTimeout*>::iterator it;
  for (it = timeouts.begin() ; it != timeouts.end(); ++it) {
    if ((*it)->deleted)
      return *it;
  }

  return NULL;
}

int EventImpl::EventToUV(int event)
{
  int ret = 0;
  if (event & VIR_EVENT_HANDLE_READABLE)
    ret |= UV_READABLE;
  if (event & VIR_EVENT_HANDLE_WRITABLE)
    ret |= UV_WRITABLE;

  return ret;
}

int EventImpl::EventFromUV(int event)
{
  int ret = 0;
  if (event & UV_READABLE)
    ret |= VIR_EVENT_HANDLE_READABLE;
  if (event & UV_WRITABLE)
    ret |= VIR_EVENT_HANDLE_WRITABLE;

  return ret;
}

} //namespace NLV
