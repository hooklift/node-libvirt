// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
#ifndef EVENTIMPL_H
#define EVENTIMPL_H

#include "node_libvirt.h"
#include "error.h"
#include <vector>

namespace NLV {

class nodeEventHandle;
class nodeEventTimeout;

class EventImpl
{
public:
  static void Initialize(Handle<Object> target);

private:
  static NAN_METHOD(SetupEvent);

  static int AddHandle(int fd, int event, virEventHandleCallback cb, void *opaque, virFreeCallback ff);
  static void UpdateHandle(int watch, int event);
  static int RemoveHandle(int watch);
  static int AddTimeout(int frequency, virEventTimeoutCallback cb, void *opaque, virFreeCallback ff);
  static void UpdateTimeout(int timer, int timeout);
  static int RemoveTimeout(int timer);

  static void HandleCallback(uv_poll_t* handle, int status, int events);

#if UV_VERSION_MAJOR < 1
  static void UpdateHandlesOnce(uv_check_t* handle, int status);
  static void CheckCallback(uv_check_t* handle, int status);
  static void TimerCallback(uv_timer_t* handle, int status);
#else
  static void UpdateHandlesOnce(uv_check_t* handle);
  static void CheckCallback(uv_check_t* handle);
  static void TimerCallback(uv_timer_t* handle);
#endif

  static void TimeoutCallback(uv_handle_s* handle);
  static void ClosePollCallback(uv_handle_t* handle);

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
};

}  //namespace NLV

#endif  // EVENTIMPL_H
