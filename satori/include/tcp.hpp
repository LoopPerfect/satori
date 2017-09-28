#ifndef SATORI_TCP_HPP
#define SATORI_TCP_HPP

#include <functional>
#include <uv.h>
#include <satori/stream.hpp>

namespace satori {

  namespace detail {

    template<class T = uv_tcp_t>
    struct Tcp : Stream<T> {
      Tcp(uv_loop_t* loop)
        : Stream<T>(loop) {
        uv_tcp_init((uv_loop_t*)loop, (uv_tcp_t*)this);
      }

      ~Tcp() {}

      int listen(char const* ip, int port, bool multi = false) {
        enableMultiProcess( ((uv_tcp_t*)this)->loop, (uv_tcp_t*)this);
    		sockaddr_in address;
    		uv_ip4_addr(ip, port, &address);
        uv_tcp_bind((uv_tcp_t*)this, (const sockaddr *) &address, 0);
        return uv_listen(
          (uv_stream_t*)this,
          1024,
          [](uv_stream_t* h, int status) {
            auto* tcp = (Tcp*)h;
          tcp->onListen(status);
        });
      }

      int keepAlive(unsigned delay = 0) {
        return uv_tcp_keepalive((uv_tcp_t*)this, !delay, delay);
      }

      std::function<void(int status)> onListen = [](int) {};
    };

  }

  using Tcp = detail::Tcp<>;
}

#endif
