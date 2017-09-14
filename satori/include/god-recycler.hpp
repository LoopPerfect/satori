#ifndef SATORI_GOD_RECYCLER_HPP
#define SATORI_GOD_RECYCLER_HPP

#include <uv.h>
#include <memory>
#include <deque>
#include <stack>
#include <functional>
#include <queue>

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
      auto const n = store.size();
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
  std::function<void()> onClose = []{};
};

struct Write : Handle {
  std::function<void(int status)> onWriteEnd = [](int){};
};

struct Stream : Handle {
  std::function<void()> onDataEnd = [](){};
  std::function<void(char* str, size_t len)> onData = [](char*, size_t){};
};

struct Tcp : Stream {
  std::function<void(int status)> onListen = [](int){};
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

    AllOfSatori(){}
    ~AllOfSatori(){}
    // add more callbacks
  } cb;

  //bool isAlive = false;


  static std::function<void(God*)> release;

  God() {
    static int i=0;
    //std::cout << i++ << std::endl;
   // assert(uv_has_ref(&uv.handle)==false);
  }


  ~God() {
    switch(type) {
      case 1:
        uv_unref(&uv.handle);
        uv.tcp.~uv_tcp_t();
        cb.tcp.~Tcp();
        return;

      case 2:
        uv.write.~uv_write_t();
        cb.write.~Write();
        return;
      default:
      case 0:
        std::cout << "?2323desxv" << std::endl;
        return;
    }
  }

  int type = 0;
  void initAsTcp(uv_loop_t* loop) {
    new (&uv) uv_tcp_t();
    new (&cb) Tcp();
    uv_tcp_init(loop, as<uv_tcp_t>());
    type = 1;
  }

  void initAsWriter(uv_loop_t*) {
    new (&uv) uv_write_t();
    new (&cb) Write();
    type = 2;
  }


  template<class T>
  T* as() {
    return (T*)this;
  }

  template<class T>
  void init() {
   // assert(!isAlive);
    new (&cb) T();
  //  isAlive = true;
  }

  constexpr int getHandleType()const {
    return uv.handle.type;
  }


};

std::function<void(God*)> God::release = [](auto){};



struct GodRecycler : Recycler<God>{
  GodRecycler(size_t const n=1024)
    : Recycler<God>(n) {
    God::release = [this](auto* g){
      release(g);
    };
  }

};


#endif
