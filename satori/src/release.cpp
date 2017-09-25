#include <satori/loop.hpp>
#include <satori/handles.hpp>

namespace satori {

  void release(Loop* loop, void* ptr) {
    loop->pool.release((AnyHandle*)ptr);
  }

}
