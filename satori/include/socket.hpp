#ifndef SATORI_SOCKET_HPP
#define SATORI_SOCKET_HPP

#include <memory>
#include <functional>

namespace Satori {

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


	      // if linux we can enable sockets to be used from multiple thread
	      assert(uv_tcp_init_ex(loop, tcp, AF_INET) == 0);
	      uv_os_fd_t fd;
	      int on = 1;
	      assert(uv_fileno((const uv_handle_t *) tcp, &fd) == 0);
	      assert(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) == 0);
	      assert(uv_tcp_bind(tcp, (const sockaddr *) &address, 0) == 0);
	      // regular code
	      //uv_tcp_bind(tcp, (sockaddr const*) &address, 0);


				uv_listen((uv_stream_t*)tcp, 128, [](uv_stream_t* h, int status) {
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

}

#endif
