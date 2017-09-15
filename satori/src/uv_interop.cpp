
#include <satori/satori.hpp>
#include <satori/uv_interop.hpp>

namespace Satori {

  void onGodWriteEnd (uv_write_t* h, int status) {
    auto* god = (God*)h;
    god->cb.write.onWriteEnd(status);
    god->release(god);
  }

  void allocBuffer(uv_handle_t* h, size_t len, uv_buf_t* buf) {
    *buf = uv_buf_init(read_buf, sizeof(read_buf));
  }

  void onGodListen(uv_stream_t* h, int status) {
    auto* god = (God*)h;
    god->cb.tcp.onListen(status);
  }

  void onGodClose(uv_handle_t* h) {
    auto* god = (God*)h;
    god->cb.handle.onClose();
    god->release(god);
  }

  void onGodRead (uv_stream_t* h, ssize_t nread, uv_buf_t const* data) {
    auto* god = (God*)h;
    if (nread < 0) {
      uv_close((uv_handle_t*)h, onGodClose);
    } else if(nread == UV_EOF) {
      god->cb.stream.onDataEnd();
    } else {
      god->cb.stream.onData(data->base, (size_t)nread);
    }
  }

  uv_buf_t createBuffer(char const* str, size_t const len) {
    uv_buf_t buf;
    buf.base = new char[len];
    buf.len = len;
    memcpy(buf.base, str, len);
    return buf;
  }

  void enableMultiProcess(uv_loop_t* loop, uv_tcp_t* server) {
    assert(uv_tcp_init_ex(loop, server, AF_INET) == 0);
    uv_os_fd_t fd;
    int on = 1;
    assert(uv_fileno((uv_handle_t*)server, &fd) == 0);
    assert(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) == 0);
  }

}
