#ifndef SATORI_GOD_RECYCLER_HPP
#define SATORI_GOD_RECYCLER_HPP

#include <uv/uv.h>
#include <memory>
#include <deque>
#include <stack>
#include <functional>

template<class T>
struct Recycler {
  std::deque<T> store;
  std::stack<T*> pool;

  Recycler(size_t const num)
    : handlers(num) {
    for(auto& x: store) {
      pool.push(&x);
    }
  }

  T* aquire() {
    if (pool.size()==0) {
      auto n = store.size();
      store.resize(n+1);
      pool.push(&store[n]);
    }
    auto g = pool.front();
    pool.pop();
    return g;
  }

  void release(T* id) {
    pool.push(id);
  }
};


struct Handle {
  std::function<void()> onClose;
};

struct Write : Handle {
  std::function<void(int)> onWriteEnd;
};

struct Stream : Handle {
  std::function<void(int status)> onReadEnd;
  std::function<void(char*)> onRead;
};



struct God {
  union {
    uv_handle_t handle;
    uv_writer_t write;
    uv_stream_t stream;
    uv_tcp_t tcp;
  } uv;

  union {
    Handle handle;
    Write write; // uv_writer_t callbacks
    Stream stream; // uv_stream_t callbacks
    // add more callbacks
  } cb;

  template<class T>
  T* as() {
    return (T*)this;
  }

  int tag;
};


/* utillity
void start(God* g) {
	uv_read_start(
		(uv_stream_t*)g,
		allocBuffer,
    [](auto* h, char const* chunk){
      auto self = (God*)h;
      self.cb.onRead(chunk);
  });
}

void stop(God* g) {
	uv_read_stop((uv_stream_t*)g);
}

*/
