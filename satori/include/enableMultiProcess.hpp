#ifndef SATORI_ENABLE_MULTI_PROCESS_HPP
#define SATORI_ENABLE_MULTI_PROCESS_HPP

#include <uv.h>

namespace satori {

  namespace detail {

    void enableMultiProcess(uv_loop_t* loop, uv_tcp_t* server);

  }
}

#endif
