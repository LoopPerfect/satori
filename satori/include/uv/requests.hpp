#ifndef SATORI_REQUESTS_HPP
#define SATORI_REQUESTS_HPP

#include <uv.h>
#include <memory>
#include <functional>
#include <satori/uv_interop.hpp>

namespace Satori {

namespace detail {

struct Request {
  Request(void*)
  {}

  void cancel() {
    uv_cancel((uv_req_t*)this);
  }
};

struct Write : Request {
  Write(void* loop)
    : Request(loop)
    , buf{0, 0}
  {}

  ~Write() {
    if(buf.len) {
      delete[] buf.base;
    }
  }

  void write(void* stream, const char* msg, size_t len) {
    if (buf.len) {
      delete[] buf.base;
    }
    buf.base = new char[len];
    buf.len = len;
    memcpy(buf.base, msg, len);
    uv_write((uv_write_t*)this, (uv_stream_t*)stream, &buf, 1, onGodWriteEnd);
  }

  std::function<void(int status)> onWriteEnd = [](int){};
  uv_buf_t buf;
};

struct FS : Request {

  FS(void* loop)
    : Request(loop) {}

  ~FS() {}
};

struct Async : Request {
  Async(void* loop)
    : Request(loop) {
    uv_async_init((uv_loop_t*)loop, (uv_async_t*)this, onGodAsync);
  }

  void start() {
    uv_async_send((uv_async_t*)this);
  }

  std::function<void()> job = []{};
};



struct Work : Request {
  Work(void* loop)
    : Request(loop) {
    uv_queue_work((uv_loop_t*)loop, (uv_work_t*)this, onGodWork, onGodWorkAfter);
  }

  std::function<void()> job = []{};
  std::function<void(int)> then =[](int){};
};

}

struct Request : uv_request_t, detail::Request {};
struct Async : uv_async_t, detail::Async {};
struct Work : uv_work_t, detail::Work {};
struct FS : uv_fs_t, detail::FS {};

}

#endif
