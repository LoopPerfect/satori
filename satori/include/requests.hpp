#ifndef SATORI_REQUESTS_HPP
#define SATORI_REQUESTS_HPP

#include <uv.h>
#include <memory>
#include <functional>
#include <string>


namespace Satori {

struct Loop;
static uv_buf_t createBuffer(char const* str, size_t const len) {
  uv_buf_t buf;
  buf.base = new char[len];
  buf.len = len;
  memcpy(buf.base, str, len);
  return buf;
}

namespace detail {

template<class T = uv_req_t>
struct Request : T {
  void* loop;

  Request(void* loop)
    : loop(loop)
  {}

  ~Request() {}

  void cancel() {
    uv_cancel((uv_req_t*)this);
  }

  void close() {
    onClose();
    release((Loop*)loop, this);
  }

  std::function<void()> onClose = []() {};
};

template<class T = uv_write_t>
struct Write : Request<T> {
  Write(void* loop)
    : Request<T>(loop)
    , buf{0, 0}
  {}

  ~Write() {
    if (buf.len) {
      delete[] buf.base;
    }
  }


  constexpr void write(void* stream, std::string const& msg) {
    write(stream, msg.c_str(), msg.size());
  }

  template<unsigned n>
  constexpr void write(void* stream, char msg[n]) {
    write(stream, msg, n);
  }

  void write(void* stream, const char* msg, size_t len) {
    if (buf.len) {
      delete[] buf.base;
    }
    buf.base = new char[len];
    buf.len = len;
    memcpy(buf.base, msg, len);
    uv_write((uv_write_t*)this, (uv_stream_t*)stream, &buf, 1, [](uv_write_t* h, int status) {
      auto* write = (Write*)h;
      write->onWriteEnd(status);
    });
  }

  std::function<void(int status)> onWriteEnd = [](int) {};
  uv_buf_t buf;
};

template<class T=uv_work_t>
struct Work : Request<T> {
  ~Work() {};
  Work(void* loop)
    : Request<T>(loop) {
    uv_queue_work(
      (uv_loop_t*)loop,
      (uv_work_t*)this,
      [](void* h) {
        auto handle =  (Work*)h;
        handle->job();
      }, [](void* h, int status) {
        auto handle =  (Work*)h;
        handle->then(status);
      }
    );
  }

  std::function<void()> job = []{};
  std::function<void(int)> then = [](int){};
};


template<class T = uv_connect_t>
struct Connect : Request<T> {
  Connect(void* loop)
    : Request<T>(loop)
  {}

  int connect(uv_tcp_t* tcp) {
    return uv_connect_t((uv_connect_t*)this, (uv_tcp_t*)tcp, [](uv_connect_t* h, int status) {
      ((Connect*) h)->onConnect(status);
    });
  }

  int connect(uv_pipe_t* pipe, char const* name) {
    return uv_pipe_connect((uv_connect_t*)this, (uv_pipe_t*)pipe, name, [](uv_connect_t* h, int status) {
      ((Connect*) h)->onConnect(status);
    });
  }

  std::function<void(int status)> onConnect = [](int){};
};


template<class T=uv_fs_t>
struct FS : Request<T> {
  FS(void* loop)
    : Request<T>(loop) {}

  ~FS() {};
};

}

using Request = detail::Request<>;
using Connect = detail::Connect<>;
using Write = detail::Write<>;
using Work = detail::Work<>;
using FS = detail::FS<>;

}

#endif
