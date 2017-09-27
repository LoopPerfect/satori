#ifndef SATORI_REQUESTS_HPP
#define SATORI_REQUESTS_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <uv.h>

//#include <satori/request.hpp>
//#include <satori/fs.hpp>

namespace satori {

template <class R> void releaseRequest(R);

static uv_buf_t createBuffer(char const* str, size_t const len) {
  uv_buf_t buf;
  buf.base = new char[len];
  buf.len = len;
  memcpy(buf.base, str, len);
  return buf;
}

template <class B> struct Request {
  void cancel() { uv_cancel((uv_req_t*)this); }
};

struct Write : uv_write_t, Request<Write> {

  std::string msg;
  Write(uv_stream_t* stream, std::string const& msg) : msg{msg} {
    write(stream);
  }

  ~Write() {}

  void write(uv_stream_t* stream) {

    uv_buf_t buf = uv_buf_init(&msg[0], msg.size());
    uv_write((uv_write_t*)this,
             (uv_stream_t*)stream,
             &buf,
             1,
             [](uv_write_t* h, int status) {
               auto* write = (Write*)h;
               write->onWriteEnd(status);
               releaseRequest(write);
             });
  }

  std::function<void(int status)> onWriteEnd = [](int) {};
};

struct ConnectTcp : uv_connect_t, Request<ConnectTcp> {
  ConnectTcp(uv_tcp_t* tcp, addrinfo const& addr) { connect(tcp, addr); }

  int connect(uv_tcp_t* tcp, addrinfo res) {
    return uv_tcp_connect(
      (uv_connect_t*)this,
      tcp,
      new sockaddr(*res.ai_addr),
      [](uv_connect_t* h, int status) { ((ConnectTcp*)h)->onConnect(status); });
  }

  std::function<void(int status)> onConnect = [](int) {};
};

struct ConnectPipe : uv_connect_t, Request<ConnectPipe> {
  ConnectPipe(uv_pipe_t* pipe, char const* name) { connect(pipe, name); }

  int connect(uv_pipe_t* pipe, char const* name) {
    uv_pipe_connect((uv_connect_t*)this,
                    (uv_pipe_t*)pipe,
                    name,
                    [](uv_connect_t* h, int status) {
                      ((ConnectPipe*)h)->onConnect(status);
                    });
    return 0;
  }

  std::function<void(int status)> onConnect = [](int) {};
};

struct GetAddrInfo : uv_getaddrinfo_t, Request<GetAddrInfo> {
  GetAddrInfo(uv_loop_t* loop, char const* host, char const* port,
              ::addrinfo hints = defaultHints()) {
    resolve(loop, host, port, hints);
  }

  static ::addrinfo defaultHints() {
    ::addrinfo hints;
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;
    return hints;
  }

  int resolve(uv_loop_t* loop, char const* host, char const* port,
              ::addrinfo hints) {
    return uv_getaddrinfo(loop,
                          (uv_getaddrinfo_t*)this,
                          GetAddrInfo::whenResolved,
                          host,
                          port,
                          &hints);
  }

  static void whenResolved(uv_getaddrinfo_t* h, int status, ::addrinfo* res) {
    ((GetAddrInfo*)h)->onResolved(status, *res);
    uv_freeaddrinfo(res);
  }

  std::function<void(int, ::addrinfo)> onResolved = [](int, ::addrinfo) {};
};

/*
    template<class T = uv_work_t>
    struct Work : Request<T> {

      Work(uv_loop_t* loop)
        : Request<T>(loop) {
        uv_queue_work(
          (uv_loop_t*)loop,
          (uv_work_t*)this,
          [](void* h) {
            auto handle = (Work*)h;
            handle->job();
          }, [](void* h, int status) {
            auto handle = (Work*)h;
            handle->then(status);
          }
        );
      }

      ~Work() {};

      std::function<void()> job = []{};
      std::function<void(int)> then = [](int) {};
    };




  using Request = detail::Request<>;
  using Connect = detail::Connect<>;
  using Write = detail::Write<>;
  using Work = detail::Work<>;
  using FS = detail::FS<>;
  using GetAddrInfo = detail::GetAddrInfo<>;*/
} // namespace satori

#endif
