#ifndef SRC_WORKER_H_
#define SRC_WORKER_H_

#include <nan.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

namespace NodeLibvirt {

typedef int (*GetNumOfType)(virConnectPtr);
typedef int (*GetListOfType)(virConnectPtr, char**, int);

class Hypervisor;
class LibVirtWorker : public NanAsyncWorker
{
public:
  explicit LibVirtWorker(NanCallback *callback, virConnectPtr connection);
  ~LibVirtWorker();

  virConnectPtr Connection() const;

  virErrorPtr VirError() const;
  void SetVirError(virErrorPtr error);

  virtual void WorkComplete();

protected:
  virtual void HandleErrorCallback();

private:
  virConnectPtr conn_;
  virErrorPtr virerror_;

};

template <typename T>
class PrimitiveReturnWorker : public LibVirtWorker
{
public:
  explicit PrimitiveReturnWorker(NanCallback *callback, virConnectPtr connection)
    : LibVirtWorker(callback, connection) {}

protected:
  void HandleOKCallback() {
    NanScope();

    v8::Local<v8::Value> argv[] = {
      NanNull(),
      NanNew(data_)
    };

    callback->Call(2, argv);
  }

  T data_;
};

template <typename T1, typename T2>
class ListReturnWorker : public LibVirtWorker
{
public:
  explicit ListReturnWorker(NanCallback *callback, virConnectPtr connection)
    : LibVirtWorker(callback, connection) {}

protected:
  virtual void HandleOKCallback() {
    NanScope();

    v8::Local<v8::Array> result = NanNew<v8::Array>(data_.size());
    for (int i = 0; i < data_.size(); ++i)
      result->Set(NanNew<v8::Integer>(i), NanNew<T2>(data_[i]));

    v8::Local<v8::Value> argv[] = {
      NanNull(),
      result
    };

    callback->Call(2, argv);
  }

  std::vector<T1> data_;
};

} // namespace NodeLibvirt

#endif // SRC_WORKER_H_
