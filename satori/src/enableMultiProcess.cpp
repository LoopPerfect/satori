#include <assert.h>
#include <uv.h>
#include <satori/enableMultiProcess.hpp>

namespace satori {
  namespace detail {

    void enableMultiProcess(uv_loop_t* loop, uv_tcp_t* server) {
      assert(uv_tcp_init_ex(loop, server, AF_INET) == 0);
      uv_os_fd_t fd;
      int on = 1;
      assert(uv_fileno((uv_handle_t*)server, &fd) == 0);
      assert(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) == 0);
    }

  }
}
