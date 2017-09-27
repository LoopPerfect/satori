#ifndef SATORI_ASYNC_HPP
#define SATORI_ASYNC_HPP

#include <functional>
#include <uv.h>
#include <satori/handle.hpp>

namespace satori {

  namespace detail {

    template<class T = uv_async_t>
    struct Async : Handle<T> {

      Async(uv_loop_t* loop, std::function<void()> f)
        : Handle<T>(loop)
        , job{f} {
        uv_async_init((uv_loop_t*)loop, (uv_async_t*)this, [](uv_async_t* h) {
          auto* handle = (Async*)h;
          handle->job();
        });
      }

      ~Async() {};

      void invoke() {
        uv_async_send((uv_async_t*)this);
      }

      std::function<void()> job = []() {};
    };

  }

  using Async = detail::Async<>;
}

#endif
