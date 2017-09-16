#ifndef SATORI_HANDLES_HPP
#define SATORI_HANDLES_HPP

#include <uv.h>
#include <memory>
#include <functional>
#include <satori/uv_interop.hpp>

namespace Satori {


struct Handle {
  Handle(void*, void* handle)
    : handle(handle)
  {}

  void* handle;
  std::function<void()> onClose = []{};
};


struct Stream : Handle {
  Stream(void* loop, void* handle)
    : Handle(loop, handle)
  {}

  std::function<void()> onDataEnd = [](){};
  std::function<void(char* str, size_t len)> onData = [](char*, size_t){};

  void read() {
    uv_read_start(
      (uv_stream_t*)handle,
      allocBuffer, onGodRead);
  }
};


struct Tcp : Stream {
  Tcp(void* loop, void* handle)
    : Stream(loop, handle) {
    uv_tcp_init((uv_loop_t*)loop, (uv_tcp_t*)handle);
  }

  void accept(Tcp* client) {
    uv_accept((uv_stream_t*)handle, (uv_stream_t*)client->handle);
  }

  void listen(char const* ip, int port, bool multi = false) {
    enableMultiProcess( ((uv_tcp_t*)handle)->loop, (uv_tcp_t*)handle);
		sockaddr_in address;
		uv_ip4_addr(ip, port, &address);
    uv_tcp_bind((uv_tcp_t*)handle, (const sockaddr *) &address, 0);
    uv_listen((uv_stream_t*)handle, 1024, onGodListen);
  }

  void close() {
    uv_close((uv_handle_t*)handle, onGodClose);
  }

  std::function<void(int status)> onListen = [](int){};
};

}

#endif
