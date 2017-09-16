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
  };

  bool isRequest = false; // uv is either a handle or a request.


  thread_local static std::function<void(God*)> release;

  God() {}

  ~God() {
      long type = uv.request.type;
      switch (type) {
        case UV_WRITE:
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
        case UV_TCP:
          uv_unref(&uv.handle);
          uv.tcp.~uv_tcp_t();
          cb.tcp.~Tcp();
          break;
        case UV_UNKNOWN_HANDLE:
          assert(false && "Unrecognized handle type");
        case UV_UNKNOWN_REQ:
          assert(false && "Unrecognized request type");
          break;
        default:
          assert(false && "god type");
          break;
    }
  }

  template<class T>
  T* as() {
    return (T*)this;
  }

  constexpr int getType() const {
    if (isRequest) {
      return request.type ;
    } else {
      return handle.type;
    }
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
