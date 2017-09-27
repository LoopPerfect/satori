#ifndef SATORI_REQUEST_HPP
#define SATORI_REQUEST_HPP

#include <functional>
#include <memory>

namespace satori {

struct Loop;

namespace detail {

template <class T = uv_req_t> struct Request : T {

  uv_loop_t *loop;

  Request(uv_loop_t *loop) : loop(loop) {}

  ~Request() {}

  void cancel() { uv_cancel((uv_req_t *)this); }

  std::function<void(int)> onError = [](int) {};
  std::function<void()> onClose = []() {};
};

} // namespace detail
} // namespace satori

#endif
