#ifndef SATORI_STREAM_HPP
#define SATORI_STREAM_HPP

#include <functional>
#include <satori/handle.hpp>
#include <uv.h>

namespace satori {

namespace detail {

thread_local static char read_buf[65635];

static void allocBuffer(uv_handle_t* h, size_t len, uv_buf_t* buf) {
  *buf = uv_buf_init(read_buf, sizeof(read_buf));
}

template <class T = uv_stream_t>
struct Stream : Handle<T> {

  Stream(uv_loop_t* loop) : Handle<T>(loop) {}

  ~Stream() {}

  int accept(void* client) {
    return uv_accept((uv_stream_t*)this, (uv_stream_t*)client);
  }

  int read() {
    return uv_read_start((uv_stream_t*)this,
                         allocBuffer,
                         [](auto* h, ssize_t nread, uv_buf_t const* data) {
                           auto* stream = (Stream*)h;
                           if (nread < 0) {
                             stream->close();
                           } else if (nread == UV_EOF) {
                             stream->onDataEnd();
                           } else {
                             stream->onData(data->base, (size_t)nread);
                           }
                         });
  }

  int stop() { return uv_read_stop((uv_stream_t*)this); }

  std::function<void()> onDataEnd = []() {};
  std::function<void(char* str, size_t len)> onData = [](char*, size_t) {};
};

} // namespace detail

using Stream = detail::Stream<>;
} // namespace satori

#endif
