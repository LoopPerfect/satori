#ifndef PLATFORM_SOCKET_HPP
#define PLATFORM_SOCKET_HPP

#include <memory>
#include <functional>

struct Socket : uv_tcp_t {
	std::function<void(Socket*, Connection*)> onConnection;
  std::function<Connection*(Socket*)> createConnection;
  std::function<void(Socket*)> onClose;

	void listen(
		char const* ip,
		int port,
		uv_loop_t* loop = uv_default_loop()) {

			uv_tcp_t* tcp = (uv_tcp_t*)this;
			uv_tcp_init(loop, tcp);
			sockaddr_in address;
			uv_ip4_addr(ip, port, &address);
			uv_tcp_bind(tcp, (sockaddr const*) &address, 0);

			uv_listen((uv_stream_t*)tcp, 1024, [](uv_stream_t* h, int status) {
				if(status) return;
				Socket* server = (Socket*)h;
				auto client = server->createConnection(server);
				uv_tcp_init(server->loop, client);
				server->onConnection(server, client);
			});
	}


	void close() {
		uv_close((uv_handle_t*)this, [](uv_handle_t* h){
       auto s = (Socket*)h;
		 	 s->onClose(s);
		});
	}
};


#endif
