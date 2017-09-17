#ifndef SATORI_HANDLES_HPP
#define SATORI_HANDLES_HPP

#include <uv.h>
#include <memory>
#include <functional>
#include <cstring>

namespace Satori {

struct Loop;

namespace detail {


thread_local static char read_buf[65635];
static void allocBuffer(uv_handle_t* h, size_t len, uv_buf_t* buf) {
  *buf = uv_buf_init(read_buf, sizeof(read_buf));
}

void enableMultiProcess(uv_loop_t* loop, uv_tcp_t* server) {
  assert(uv_tcp_init_ex(loop, server, AF_INET) == 0);
  uv_os_fd_t fd;
  int on = 1;
  assert(uv_fileno((uv_handle_t*)server, &fd) == 0);
  assert(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) == 0);
}

template<class T=uv_handle_t>
struct Handle : T {

  ~Handle() {}
  Handle(void*) {}

  int close() {
    uv_close((uv_handle_t*)this, [](auto* h){
      auto* handle = (Handle*)h;
      handle->onClose();
      ((Loop*)handle->loop)
        ->release(handle);
    });
    return 0;
  }
  std::function<void()> onClose = []{};
};


template<class T=uv_stream_t>
struct Stream : Handle<T> {

  Stream(void* loop)
    : Handle<T>(loop)
  {}
  ~Stream() {}

  int accept(void* client) {
    return uv_accept((uv_stream_t*)this, (uv_stream_t*)client);
  }

  int read() {
    return uv_read_start(
      (uv_stream_t*)this,
      allocBuffer,
      [](auto* h, ssize_t nread, uv_buf_t const* data) {
        auto* stream = (Stream*)h;
        if (nread < 0) {
          stream->close();
        } else if(nread == UV_EOF) {
          stream->onDataEnd();
        } else {
          stream->onData(data->base, (size_t)nread);
        }
      });
  }


  int stop() {
    return uv_read_stop(
      (uv_stream_t*)this);
  }

  std::function<void()> onDataEnd = [](){};
  std::function<void(char* str, size_t len)> onData = [](char*, size_t){};
};


template<class T=uv_tcp_t>
struct Tcp : Stream<T> {
  Tcp(void* loop)
    : Stream<T>(loop) {
    uv_tcp_init((uv_loop_t*)loop, (uv_tcp_t*)this);
  }
  ~Tcp() {}

  int listen(char const* ip, int port, bool multi = false) {
    enableMultiProcess( ((uv_tcp_t*)this)->loop, (uv_tcp_t*)this);
		sockaddr_in address;
		uv_ip4_addr(ip, port, &address);
    uv_tcp_bind((uv_tcp_t*)this, (const sockaddr *) &address, 0);
    return uv_listen(
      (uv_stream_t*)this,
      1024,
      [](uv_stream_t* h, int status) {
        auto* tcp = (Tcp*)h;
      tcp->onListen(status);
    });
  }

  std::function<void(int status)> onListen = [](int){};
};

template<class T=uv_async_t>
struct Async : Handle<T> {
  Async(void* loop, std::function<void()> f)
    : Handle<T>(loop)
    , job{f} {
    uv_async_init((uv_loop_t*)loop, (uv_async_t*)this, [](void* h) {
        auto handle = (Async*)h;
        handle->job();
    });
  }

  ~Async() {};

  void invoke() {
    uv_async_send((uv_async_t*)this);
  }

  std::function<void()> job = [](auto){};
};

}

using Handle = detail::Handle<>;
using Stream = detail::Stream<>;
using Tcp = detail::Tcp<>;

using Async = detail::Async<>;

}

#endif
