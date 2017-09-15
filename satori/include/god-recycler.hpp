#ifndef SATORI_GOD_RECYCLER_HPP
#define SATORI_GOD_RECYCLER_HPP

#include <uv.h>
#include <assert.h>
#include <memory>
#include <deque>
#include <stack>
#include <functional>
#include <queue>
#include <cstring>

#include <satori/recycler.hpp>
#include <satori/uv_interop.hpp>


namespace Satori {

struct Handle {
  Handle(void*, void* handle)
    : handle(handle)
  {}

  void* handle;
  std::function<void()> onClose = []{};
};

struct Async : Handle {
  Async(void* loop, void* handle)
    : Handle(loop, handle) {
    uv_async_init((uv_loop_t*)loop, (uv_async_t*)handle, onGodAsync);
  }

  void start() {
    uv_async_send((uv_async_t*)handle);
  }

  std::function<void()> job = []{};
};

struct Work : Handle {
  Work(void* loop, void* handle)
    : Handle(loop, handle) {
    uv_queue_work((uv_loop_t*)loop, (uv_work_t*)handle, onGodWork, onGodWorkAfter);
  }

  void cancel() {
    uv_cancel((uv_req_t*)handle);
  }

  std::function<void()> job = []{};
  std::function<void(int)> then =[](int){};

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

struct Write : Handle {
  std::function<void(int status)> onWriteEnd = [](int){};
  uv_buf_t buf;

  Write(void* loop, void* handle)
    : Handle(loop, handle)
    , buf{0, 0}
  {}

  ~Write() {
    if(buf.len) {
      delete[] buf.base;
    }
  }

  void close() {}

  void write(Stream* stream, const char* msg, size_t len) {
    if (buf.len) {
      delete[] buf.base;
    }
    buf.base = new char[len];
    buf.len = len;
    memcpy(buf.base, msg, len);
    uv_write((uv_write_t*)handle, (uv_stream_t*)stream->handle, &buf, 1, onGodWriteEnd);
  }

};

struct FS : Handle {

  FS(void* loop, void* handle)
    : Handle(loop, handle) {}

  ~FS() {}

  void close() {}
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

struct God {

  union UV {

    // Handles
    uv_handle_t handle;
    uv_stream_t stream;
    uv_tcp_t tcp;
    // Requests
    uv_req_t request;
    uv_write_t write;
    uv_fs_t fs;
    uv_work_t work;
    uv_async_t async;

    UV() {}
    ~UV() {}
  } uv;

  bool isRequest = false; // uv is either a handle or a request.

  union Satori {
    Handle handle;
    Async async;
    Write write; // uv_writer_t callbacks
    Stream stream; // uv_stream_t callbacks
    Tcp tcp;
    FS fs;
    Work work;
    Satori() {}
    ~Satori() {}
  } cb;

  thread_local static std::function<void(God*)> release;

  God() {}

  ~God() {
    if (isRequest) {
      uv_req_type type = uv.request.type;
      switch (type) {
        case UV_WRITE:
          uv.write.~uv_write_t();
          cb.write.~Write();
          break;
        case UV_FS:
          uv.fs.~uv_fs_t();
          cb.fs.~FS();
          break;
        case UV_ASYNC:
          uv_unref(&uv.handle);
          uv.async.~uv_async_t();
          cb.async.~Async();
          break;
        case UV_WORK:
          uv_unref(&uv.handle);
          uv.work.~uv_work_t();
          cb.work.~Work();
          break;
        case UV_UNKNOWN_REQ:
        default:
          assert(false && "Unrecognized request type");
      }
    } else {
      uv_handle_type type = uv.handle.type;
      switch (type) {
        case UV_TCP:
          uv_unref(&uv.handle);
          uv.tcp.~uv_tcp_t();
          cb.tcp.~Tcp();
          break;
        case UV_UNKNOWN_HANDLE:
        default:
          assert(false && "Unrecognized handle type");
      }
    }
  }

  void initAsTcp(uv_loop_t* loop) {
    new (&uv) uv_tcp_t();
    new (&cb) Tcp(loop, this);
    isRequest = false;
  }

  void initAsFS(uv_loop_t* loop) {
    new (&uv) uv_fs_t();
    new (&cb) FS(loop, this);
    isRequest = true;
  }

  void initAsWriter(uv_loop_t* loop) {
    new (&uv) uv_write_t();
    new (&cb) Write(loop, this);
    isRequest = true;
  }

  void initAsAsync(uv_loop_t* loop) {
    new (&uv) uv_async_t();
    new (&cb) Async(loop, this);
    isRequest = true;
  }

  void initAsWork(uv_loop_t* loop) {
    new (&uv) uv_work_t();
    new (&cb) Work(loop, this);
    isRequest = true;
  }

  template<class T>
  T* as() {
    return (T*)this;
  }

  constexpr int getHandleType() const {
    assert(!isRequest && "getHandleType is not valid for requests");
    return uv.handle.type;
  }
};



struct GodRecycler : Recycler<God>{
  GodRecycler(size_t const n = 1024)
    : Recycler<God>(n) {
    God::release = [this](auto* g){
      release(g);
    };
  }
};

}

#endif
