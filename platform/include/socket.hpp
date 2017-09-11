#ifndef PLATFORM_SOCKET_HPP
#define PLATFORM_SOCKET_HPP

#include <memory>
#include <functional>

struct Socket : uv_tcp_t {
	std::function<void(Socket*, Connection*)> onConnection;

	template<class G>
	Socket(G&&g)
		: onConnection(std::forward<G>(g))
	{}

	template<class G>
	static Socket* create(G&&g) {
		return new Socket{std::forward<G>(g)};
	}

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
				auto client = new Connection((uv_tcp_t*)server);
				uv_tcp_init(server->loop, client);
				server->onConnection(server, client);
			});
	}


	void close() {
		uv_close((uv_handle_t*)this, [](uv_handle_t* s){
			delete (Socket*)s;
		});
	}
};


#endif
