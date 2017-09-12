#ifndef PLATFORM_ALLOCATORS_HPP
#define PLATFORM_ALLOCATORS_HPP

#include <memory>
#include <vector>
#include <queue>
#include <algorithm>

#include <platform/writer.hpp>
#include <platform/socket.hpp>

template<class Handle>
struct Recycler {
  std::vector<std::unique_ptr<Handle>> handlers;
  std::queue<Handle*> pool;

  Recycler(size_t const num)
    : handlers(num)
  {
    std::generate(handlers.begin(), handlers.end(), []{
      return std::make_unique<Handle>(); //there is probably a better way to initialize ...
    });
    for(auto& w: handlers) {
      pool.push(w.get());
    }
  }

  Handle* get() {
    if (pool.size()==0) {  //how should we grow ?
      handlers.emplace_back(std::make_unique<Handle>());
      pool.push( handlers[handlers.size()-1].get() );
    }
    auto w = pool.front();
    pool.pop();
    return w;
  }

  void release(Handle* w) {
    pool.push(w);
  }
};



struct BufferFactory {
  static uv_buf_t create(size_t len) {
    uv_buf_t b;
    b.base = new char[len];
    b.len = len;
    return b;
  }

  static void release(uv_buf_t& b) {
    delete[] b.base;
    b.len = 0;
  }
};

template<class BMM = BufferFactory>
struct WriterFactory {

  BMM bmm;
  WriterFactory(BMM bmm = BMM())
    : bmm{bmm}
  {}

  Writer* create(
      uv_stream_t* stream,
      std::function<void(Writer*, int)> then
  ) {
    auto w = new Writer;

    w->getBuffer = [this](size_t len) {
      return bmm.create(len);
    };

    w->releaseBuffer = [this](uv_buf_t& b) {
      bmm.release(b);
    };

    w->stream = stream;
    w->then = then;
    w->onClose = [this](auto w){ release(w); };
    return w;
  };

  void release(Writer* w) {
    delete w;
  }
};



template<class BMM = BufferFactory>
struct WriterPool : Recycler<Writer> {
  BMM bmm;

  WriterPool(size_t const num = 1024, BMM bmm = BMM())
    : Recycler<Writer>(num)
    , bmm(bmm)
  {}

  Writer* create(
    uv_stream_t* stream,
    std::function<void(Writer*, int)> then) {
    auto w = get();
    w->stream = stream;
    w->then = then;
    w->onClose = [this](auto w){ release(w);};

    w->getBuffer = [this](size_t len) {
      return bmm.create(len);
    };

    w->releaseBuffer = [this](uv_buf_t& b) {
      bmm.release(b);
    };

    return w;
  };
};


struct ConnectionPool : Recycler<Connection> {
  ConnectionPool(size_t num=1024)
    : Recycler<Connection>(num)
  {}

  Connection* create(uv_stream_t* server) {
    auto s = get();
    s->server = (uv_tcp_t*)server;
    s->onClose = [this](auto s){
      release(s);
    };
    return s;
  };
};


struct SocketPool : Recycler<Socket> {
  ConnectionPool cp;
  SocketPool(size_t num = 1024)
    : Recycler<Socket>(1)
    , cp(num)
  {}

  template<class F>
  Socket* create(F f) {
    auto server = get();
    server->onConnection = f;
    server->onClose = [this](Socket* s) { release(s); };
    server->createConnection = [this](auto s){
      return cp.create((uv_stream_t*)s);
    };

    return server;
  }
};




#endif
