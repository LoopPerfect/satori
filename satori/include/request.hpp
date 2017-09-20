#ifndef SATORI_REQUEST_HPP
#define SATORI_REQUEST_HPP

#include <memory>
#include <functional>

namespace Satori {

  struct Loop;

  namespace detail {

    template<class T = uv_req_t>
    struct Request : T {

      uv_loop_t* loop;

      Request(uv_loop_t* loop)
        : loop(loop)
      {}

      ~Request() {}

      void cancel() {
        uv_cancel((uv_req_t*)this);
      }

      void close() {
        onClose();
        release((Loop*)loop, this);
      }

      std::function<void()> onClose = []() {};
    };

  }
}

#endif
