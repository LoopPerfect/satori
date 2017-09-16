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
#include <satori/handles.hpp>
#include <satori/requests.hpp>


namespace Satori {


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
