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


#include <satori/http-parser.hpp>
#include <satori/satori.hpp>

thread_local static char read_buf[66536];


static void allocBuffer(uv_handle_t *h, size_t len, uv_buf_t *buf) {
	*buf = uv_buf_init(read_buf, sizeof(read_buf));
}

void enableMultiProcess(uv_loop_t* loop, uv_tcp_t* server) {
  assert(uv_tcp_init_ex(loop, server, AF_INET) == 0);
  uv_os_fd_t fd;
  int on = 1;
  assert(uv_fileno((uv_handle_t*)server, &fd) == 0);
  assert(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) == 0);
}


void onGodListen (uv_stream_t* h, int status) {
  auto* god = (God*)h;
  god->cb.tcp.onListen(status);
}

void onGodClose (uv_handle_t* h) {
  auto* god = (God*)h;
  god->cb.handle.onClose();
  god->release();
}

void onGodWriteEnd (uv_write_t* h, int status) {
  auto* god = (God*)h;
  god->cb.write.onWriteEnd(status);
  god->release();
}


void onGodRead (uv_stream_t* h, ssize_t nread, uv_buf_t const* data) {
  auto* god = (God*)h;
	if(nread < 0) {
		god->cb.stream.onClose();
    god->release();
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
  using namespace Satori;

  auto gr = std::make_shared<GodRecycler>();
  auto server = gr->take();


  auto* loop = uv_default_loop();
  server->initAsTcp(loop);


  server->cb.tcp.onListen = [=](auto status) {
    auto client = gr->take();
    client->initAsTcp(loop);

    uv_accept(server->as<uv_stream_t>() , client->as<uv_stream_t>());


    client->cb.tcp.onData = [=](char const* data, size_t const len) {
      //std::cout << str << std::endl;
      auto req = parseReq(std::string(data, data+len));

      if(req.size() == 0) std::cout << "DSADAS" << std::endl;

      auto writer = gr->take();
      writer->initAsWriter(loop);

      char res[] =
        "HTTP/1.1 200 OK\r\n"
          "Connection: keep-alive\r\n"
          "Content-Type: text/plain\r\n"
          "\r\n"
          "Hello World!";
      uv_buf_t buf = createBuffer(res, sizeof(res));

      writer->cb.write.onWriteEnd = [=](int status) {
        uv_close(client->as<uv_handle_t>(), onGodClose);
        delete[] buf.base;
      };

      uv_write(writer->as<uv_write_t>(), client->as<uv_stream_t>(), &buf, 1, onGodWriteEnd);

    };

    client->cb.tcp.onDataEnd = [=] {
      std::cout << "data end" << std::endl;
    };


    uv_read_start(
        client->as<uv_stream_t>(),
        allocBuffer, onGodRead);

  };


  sockaddr_in address;
  uv_ip4_addr("127.0.0.1", 8081, &address);

  enableMultiProcess(loop, server->as<uv_tcp_t>());
  uv_tcp_bind(server->as<uv_tcp_t>(), (const sockaddr *) &address, 0);


  uv_listen(server->as<uv_stream_t>(), 1024, onGodListen);

  uv_run(loop, UV_RUN_DEFAULT);

  // loop.close();
}
