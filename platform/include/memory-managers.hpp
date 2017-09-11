#ifndef PLATFORM_ALLOCATORS_HPP
#define PLATFORM_ALLOCATORS_HPP

#include <memory>
#include <vector>
#include <queue>

#include <platform/writer.hpp>
#include <platform/socket.hpp>

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
struct RecyclingWriter {
  BMM bmm;
  std::vector<Writer> writers;
  std::queue<Writer*> pool;

  RecyclingWriter(size_t const num, BMM bmm = BMM())
    : bmm(bmm)
    , writers(num)
  {
    for(auto& w: writers) {
      pool.push(&w);
    }
  }

  Writer* get() {
    auto w = pool.front();
    pool.pop();
    return w;
  }

  Writer* create(
      uv_stream_t* stream,
      std::function<void(Writer*, int)> then
  ) {
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

  void release(Writer* w) {
    pool.push(w);
  }
};


struct


#endif
