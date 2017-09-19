#include <satori/loop.hpp>
#include <satori/handles.hpp>

namespace Satori {

  void release(Loop* loop, void* ptr) {
    loop->pool.release((AnyHandle*)ptr);
  }

}
