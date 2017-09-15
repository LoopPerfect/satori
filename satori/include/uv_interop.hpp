#ifndef SATORI_UV_INTEROP_HPP
#define SATORI_UV_INTEROP_HPP

#include <uv.h>

namespace Satori {
  thread_local static char read_buf[66536];

  void enableMultiProcess(uv_loop_t* loop, uv_tcp_t* server);
  void allocBuffer(uv_handle_t* h, size_t len, uv_buf_t* buf);
  void onGodWriteEnd(uv_write_t* h, int status);
  void onGodListen(uv_stream_t* h, int status);
  void onGodClose(uv_handle_t* h);
  void onGodAsync(uv_async_t* h);
  void onGodRead(uv_stream_t* h, ssize_t nread, uv_buf_t const* data);

  uv_buf_t createBuffer(char const* str, size_t const len);
}

#endif
