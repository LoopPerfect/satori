#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory>
#include <functional>
#include <type_traits>
#include <queue>
#include <string.h>
#include <unistd.h>

#include <uv.h>

#include <satori/satori.hpp>

thread_local static char read_buf[66536];
static void allocBuffer(uv_handle_t *h, size_t len, uv_buf_t *buf) {
	*buf = uv_buf_init(read_buf, sizeof(read_buf));
}



void onGodListen (uv_stream_t* h, int status) {
  auto* god = (God*)h;
  god->cb.tcp.onListen(status);
}

void onGodClose (uv_handle_t* h) {
  auto* god = (God*)h;
  god->cb.handle.onClose();
  god->isAlive = false;
}

void onGodWriteEnd (uv_write_t* h, int status) {
  auto* god = (God*)h;
  god->cb.write.onWriteEnd(status);
  god->isAlive = false;
}


void onGodRead (uv_stream_t* h, ssize_t nread, uv_buf_t const* data) {
  auto* god = (God*)h;
	if(nread < 0) {
    god->isAlive = false;
		god->cb.stream.onClose();
	} else if(nread == UV_EOF) {
		god->cb.stream.onDataEnd();
	} else {
		god->cb.stream.onData(data->base, (size_t)nread);
	}
}


uv_buf_t createBuffer(char const* str, size_t const len) {
  uv_buf_t buf;
  buf.base = new char[len];
  buf.len = len;
  memcpy(buf.base, str, len);
  return buf;
}

int main() {

  using namespace std;

  GodRecycler gr;
  auto server = gr.take();


  auto* loop = uv_default_loop();
  server->initAsTcp(loop);


  server->cb.tcp.onListen = [&](auto status) {
    std::cout << status << std::endl;
    auto client = gr.take();
    client->initAsTcp(loop);

    uv_accept(server->as<uv_stream_t>() , client->as<uv_stream_t>());


    client->cb.tcp.onData = [&](char const* str, size_t const len) {
      std::cout << str << std::endl;

      auto writer = gr.take();
      writer->initAsWriter(loop);

      char res[] = "Hello World";
      auto buf = createBuffer(res, sizeof(res));

      writer->cb.write.onWriteEnd = [&](int status) {
        uv_close(client->as<uv_handle_t>(), onGodClose);
      };

      uv_write(writer->as<uv_write_t>(), client->as<uv_stream_t>(), &buf, 1, onGodWriteEnd);

    };

    client->cb.tcp.onDataEnd = [&] {
      std::cout << "data end" << std::endl;
    };


    uv_read_start(
        client->as<uv_stream_t>(),
        allocBuffer, onGodRead);

  };


  sockaddr_in address;
  uv_ip4_addr("127.0.0.1", 8081, &address);
  uv_tcp_bind(server->as<uv_tcp_t>(), (const sockaddr *) &address, 0);


  uv_listen(server->as<uv_stream_t>(), 128, onGodListen);

  uv_run(loop, UV_RUN_DEFAULT);

  // loop.close();
}
