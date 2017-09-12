#ifndef PLATFORM_WRITER_HPP
#define PLATFORM_WRITER_HPP

#include<uv.h>
#include<functional>
#include<memory>
#include<string.h>



struct Writer : uv_write_t {
	uv_stream_t* stream = nullptr;
	std::function<void(Writer*, int)> then = [](auto, auto){};
  std::function<void()> error = [](){};
  std::function<void(Writer*)> onClose;

  std::function<uv_buf_t(size_t)> getBuffer;
  std::function<void(uv_buf_t&)> releaseBuffer;

  uv_buf_t buf = {};

  void write(char const* str, size_t len) {
    if(buf.len)
      releaseBuffer(buf);
    buf = getBuffer(len);
    memcpy(buf.base, str, len);
    buf.len=len;
    uv_write(this, stream, &buf, 1, Writer::call);
	}

 	static void call(uv_write_t* h, int status) {
    if (status != UV_ECANCELED) {
      auto handle = (Writer*)h;
      handle->then(handle, status);
    } else {
      auto handle = (Writer*)h;
      handle->then(handle, status);
    }
  }

  void close() {
    onClose(this);
  }

  ~Writer() {
    if(buf.len)
      releaseBuffer(buf);
  }
};

#endif
