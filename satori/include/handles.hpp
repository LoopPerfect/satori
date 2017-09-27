#ifndef SATORI_HANDLES_HPP
#define SATORI_HANDLES_HPP

#include <assert.h>
#include <cstring>
#include <functional>
#include <memory>

#include <uv.h>

#include <satori/enableMultiProcess.hpp>
//#include <satori/process.hpp>

namespace satori {

thread_local static char read_buf[65635];
static void allocBuffer(uv_handle_t* h, size_t len, uv_buf_t* buf) {
  *buf = uv_buf_init(read_buf, sizeof(read_buf));
}

template <class H>
void release(H);

struct HandleCB {
  std::function<void()> onClose = [] {};
};

struct StreamCB : HandleCB {
  std::function<void()> onDataEnd = [] {};
  std::function<void(char* str, size_t len)> onData = [](char*, size_t) {};
};

struct TcpCB : StreamCB {
  std::function<void(int status)> onListen = [](int) {};
};

struct AsyncCB : HandleCB {
  std::function<void()> job = []() {};
};

template <class B>
struct Handle {
  int close() {
    uv_close((uv_handle_t*)this, [](auto* h) {
      auto* handle = (B*)h;
      handle->onClose();
      release(handle);
    });
    return 0;
  }
};

template <class B>
struct Stream : Handle<B> {

  int accept(void* client) {
    return uv_accept((uv_stream_t*)this, (uv_stream_t*)client);
  }

  int read() {
    return uv_read_start((uv_stream_t*)this,
                         allocBuffer,
                         [](auto* h, ssize_t nread, uv_buf_t const* data) {
                           auto* stream = (B*)h;
                           if (nread < 0) {
                             stream->close();
                           } else if (nread == UV_EOF) {
                             stream->onDataEnd();
                           } else {
                             stream->onData(data->base, (size_t)nread);
                           }
                         });
  }

  int stop() { return uv_read_stop((uv_stream_t*)this); }
};

struct Tcp : uv_tcp_t, Stream<Tcp>, TcpCB {

  Tcp(uv_loop_t* loop) { uv_tcp_init((uv_loop_t*)loop, (uv_tcp_t*)this); }

  ~Tcp() {}

  int listen(char const* ip, int port, bool multi = false) {
    enableMultiProcess(((uv_tcp_t*)this)->loop, (uv_tcp_t*)this);
    sockaddr_in address;
    uv_ip4_addr(ip, port, &address);
    uv_tcp_bind((uv_tcp_t*)this, (const sockaddr*)&address, 0);
    return uv_listen((uv_stream_t*)this, 1024, [](uv_stream_t* h, int status) {
      auto* tcp = (Tcp*)h;
      tcp->onListen(status);
    });
  }

  int keepAlive(unsigned delay = 0) {
    return uv_tcp_keepalive((uv_tcp_t*)this, !delay, delay);
  }
};

struct Pipe : uv_pipe_t, Stream<Pipe>, StreamCB {

  Pipe(uv_loop_t* loop, bool ipc = 0) {
    uv_pipe_init((uv_loop_t*)loop, (uv_pipe_t*)this, ipc);
  }

  int bindToFile(uv_file file) { return uv_pipe_open((uv_pipe_t*)this, file); }

  int bindToName(const char* name) {
    return uv_pipe_bind((uv_pipe_t*)this, name);
  }

  ~Pipe() {}
};

struct Async : uv_async_t, Handle<Async>, AsyncCB {

  Async(uv_loop_t* loop) {
    uv_async_init((uv_loop_t*)loop, (uv_async_t*)this, [](uv_async_t* h) {
      auto handle = (Async*)h;
      handle->job();
    });
  }

  ~Async(){};

  void invoke() { uv_async_send((uv_async_t*)this); }
};

} // namespace satori

#endif
