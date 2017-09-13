#ifndef SATORI_GOD_RECYCLER_HPP
#define SATORI_GOD_RECYCLER_HPP

#include <uv.h>
#include <memory>
#include <deque>
#include <stack>
#include <functional>

template<class T>
struct Recycler {
  std::deque<T> store;
  std::stack<T*> pool;

  Recycler(size_t const num)
    : store(num) {
    for(auto& x: store) {
      pool.push(&x);
    }
  }

  T* take() {
    if (pool.size()==0) {
      auto n = store.size();
      store.emplace_back();
      return &store[n];
    }
    auto g = pool.top();
    pool.pop();
    return g;
  }

  void release(T* o) {
    o->~T();
    pool.push(o);
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

struct Tcp : Stream {
  std::function<void(int status)> onListen;
};


struct God {
  union AllOfUV {
    uv_handle_t handle;
    uv_write_t write;
    uv_stream_t stream;
    uv_tcp_t tcp;
    AllOfUV(){}
    ~AllOfUV(){}
  } uv;

  union AllOfSatori {
    Handle handle;
    Write write; // uv_writer_t callbacks
    Stream stream; // uv_stream_t callbacks
    Tcp tcp;

    AllOfSatori(){
      handle.onClose = []{};
    }
    ~AllOfSatori(){}
    // add more callbacks
  } cb;

  bool isAlive = false;


  God() {}


  ~God() {
    //TODO
  }


  void initAsTcp(uv_loop_t* loop) {
    assert(!isAlive);
    isAlive = true;
    uv_tcp_init(loop, this->as<uv_tcp_t>());
    this->init<Tcp>();
  }


  template<class T>
  T* as() {
    return (T*)this;
  }

  template<class T>
  void init() {
    new (&cb) T();
  }

  constexpr int getType()const {
    return uv.handle.type;
  }


};


struct GodRecycler : Recycler<God>{
  GodRecycler(size_t const n=1024)
    : Recycler<God>(n)
  {}
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

#endif
