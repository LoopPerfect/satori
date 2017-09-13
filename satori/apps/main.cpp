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

void onGodListen (uv_stream_t* h, int status) {
  auto* god = (God*)h;
  god->cb.tcp.onListen(status);
}

void onGodClose (uv_handle_t* h) {
  auto* god = (God*)h;
  god->cb.handle.onClose();
  god->isAlive = false;
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

    uv_accept(server->as<uv_tcp_t>() , client->as<uv_tcp_t>());
    uv_close(client->as<uv_stream_t>(), onGodClose);
  };


  sockaddr_in address;
	uv_ip4_addr("127.0.0.1", 8081, &address);
  uv_tcp_bind(server->as<uv_tcp_t>(), (const sockaddr *) &address, 0);


  uv_listen(server->as<uv_stream_t>(), 128, onGodListen);

	uv_run(loop, UV_RUN_DEFAULT);

  // loop.close();
}
