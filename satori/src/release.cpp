#include <satori/handles.hpp>
#include <satori/loop.hpp>

namespace satori {

void release(Loop *loop, void *ptr) { loop->pool.release(ptr); }

} // namespace satori
