#ifndef SATORI_REQUESTS_HPP
#define SATORI_REQUESTS_HPP

#include <uv.h>
#include <memory>
#include <functional>
#include <satori/uv_interop.hpp>

namespace Satori {

struct Request {
  Request(void*, void* handle)
    : handle(handle)
  {}

  void cancel() {
    uv_cancel((uv_req_t*)handle);
  }

  void* handle;
};

struct Write : Request {
  Write(void* loop, void* handle)
    : Request(loop, handle)
    , buf{0, 0}
  {}

  ~Write() {
    if(buf.len) {
      delete[] buf.base;
    }
  }

  void write(Stream* stream, const char* msg, size_t len) {
    if (buf.len) {
      delete[] buf.base;
    }
    buf.base = new char[len];
    buf.len = len;
    memcpy(buf.base, msg, len);
    uv_write((uv_write_t*)handle, (uv_stream_t*)stream->handle, &buf, 1, onGodWriteEnd);
  }

  std::function<void(int status)> onWriteEnd = [](int){};
  uv_buf_t buf;
};

struct FS : Request {

  FS(void* loop, void* handle)
    : Request(loop, handle) {}

  ~FS() {}

};



struct Async : Request {
  Async(void* loop, void* handle)
    : Request(loop, handle) {
    uv_async_init((uv_loop_t*)loop, (uv_async_t*)handle, onGodAsync);
  }

  void start() {
    uv_async_send((uv_async_t*)handle);
  }

  std::function<void()> job = []{};
};


struct Work : Request {
  Work(void* loop, void* handle)
    : Request(loop, handle) {
    uv_queue_work((uv_loop_t*)loop, (uv_work_t*)handle, onGodWork, onGodWorkAfter);
  }

  std::function<void()> job = []{};
  std::function<void(int)> then =[](int){};
};

}

#endif
