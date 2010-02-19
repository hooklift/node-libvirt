#include <node.h>
#include <node_events.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

using namespace v8;
using namespace node;

class Connection : public EventEmitter {
 public:
  static void Initialize (v8::Handle<v8::Object> target) {

  }

extern "C" void init (Handle<Object> target) {
  HandleScope scope;
  Connection::Initialize(target);
}
