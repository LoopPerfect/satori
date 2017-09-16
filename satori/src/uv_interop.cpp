
//#include <satori/satori.hpp>
#include <uv.h>
#include <satori/uv_interop.hpp>
#include <satori/handles.hpp>
#include <satori/requests.hpp>
#include <cassert>
#include <iostream>

namespace Satori {

  void onGodAsync (uv_async_t* h) {
    auto* async = (Async*)h;
    async->job();
  }

  void onGodWork(uv_work_t* h) {
    auto* work = (Work*)h;
    work->job();
  }

  void onGodWorkAfter(uv_work_t* h, int status) {
    auto* work = (Work*)h;
    work->then(status);
  }

  void onGodWriteEnd (uv_write_t* h, int status) {
    auto* write = (Write*)h;
    write->onWriteEnd(status);
  }

  void allocBuffer(uv_handle_t* h, size_t len, uv_buf_t* buf) {
    *buf = uv_buf_init(read_buf, sizeof(read_buf));
  }

  void onGodListen(uv_stream_t* h, int status) {
    std::cout << h << std::endl;
    auto* tcp = (Tcp*)h;
    tcp->onListen(status);
  }

  void onGodClose(uv_handle_t* h) {
    auto* handle = (Handle*)h;
    handle->onClose();
    //handle->release(handle);
  }

  void onGodRead (uv_stream_t* h, ssize_t nread, uv_buf_t const* data) {
    auto* stream = (Stream*)h;
    if (nread < 0) {
      uv_close((uv_handle_t*)h, onGodClose);
    } else if(nread == UV_EOF) {
      stream->onDataEnd();
    } else {
      stream->onData(data->base, (size_t)nread);
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
