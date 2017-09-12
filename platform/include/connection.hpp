#ifndef PLATFORM_CONNECTION_HPP
#define PLATFORM_CONNECTION_HPP

#include <uv.h>
#include <functional>
#include <platform/writer.hpp>

//works only on unix. On Linux reads on sockets never interleave
thread_local static char read_buf[66536];

struct Connection : uv_tcp_t {
	uv_tcp_t* server;
	std::function<void(Connection*, char const*)> onData;
	std::function<void(Connection*)> onDataEnd;
  std::function<void(Connection*)> onClose;


	Connection(uv_tcp_t* server = nullptr)
		:server{server}
	{}

	void read() {
		uv_read_start(
			(uv_stream_t*)this,
			Connection::allocBuffer, Connection::onRead);
	}

	void stopReading() {
		uv_read_stop((uv_stream_t*)this);
	}

	template<class F>
	auto write(char const* str, ssize_t len, F&&f) {
		auto writer = new Writer{};
    writer->stream = (uv_stream_t*)this;
    writer->then = f;
		writer->write(str, len);
		return writer;
	}

	bool accept() {
		return uv_accept((uv_stream_t*)server, (uv_stream_t*)this);
	}

	void close() {
		uv_close((uv_handle_t*)this, [](uv_handle_t* h) {
			auto c = (Connection*)h;
		  c->onClose(c);
		});
	}

  static void onRead(uv_stream_t* h, ssize_t nread, uv_buf_t const* data){
		auto c = (Connection*)h;
		if(nread < 0) {
			c->close();
		} else if(nread == UV_EOF) {
			c->onDataEnd(c);
		} else {
			c->onData(c, data->base);
		}
	}

  static void allocBuffer(uv_handle_t *h, size_t len, uv_buf_t *buf) {
    *buf = uv_buf_init(read_buf, sizeof(read_buf));
  }


};

#endif
