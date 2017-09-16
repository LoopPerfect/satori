#ifndef SATORI_HANDLES_HPP
#define SATORI_HANDLES_HPP

#include <uv.h>
#include <memory>
#include <functional>
#include <satori/uv_interop.hpp>

namespace Satori {

namespace detail {

struct Handle {
  Handle(void*)
  {}

  std::function<void()> onClose = []{};
};


struct Stream : Handle {
  Stream(void* loop)
    : Handle(loop)
  {}

  void accept(void* client) {
    uv_accept((uv_stream_t*)this, (uv_stream_t*)client);
  }

  void read() {
    uv_read_start(
      (uv_stream_t*)this,
      allocBuffer, onGodRead);
  }


  void stop() {
    uv_read_stop(
      (uv_stream_t*)this);
  }

  std::function<void()> onDataEnd = [](){};
  std::function<void(char* str, size_t len)> onData = [](char*, size_t){};
};

struct Tcp {
  Tcp(void* loop)
    : Stream(loop) {
    uv_tcp_init((uv_loop_t*)loop, (uv_tcp_t*)this);
  }

  void listen(char const* ip, int port, bool multi = false) {
    enableMultiProcess( ((uv_tcp_t*)this)->loop, (uv_tcp_t*)this);
		sockaddr_in address;
		uv_ip4_addr(ip, port, &address);
    uv_tcp_bind((uv_tcp_t*)this, (const sockaddr *) &address, 0);
    uv_listen((uv_stream_t*)this, 1024, onGodListen);
  }

  std::function<void(int status)> onListen = [](int){};
};

}

struct Handle : uv_handle_t, detail::Handle {};
struct Stream : uv_stream_t, detail::Stream {};
struct Tcp : uv_tcp_t, detail::Tcp {};




}

#endif
