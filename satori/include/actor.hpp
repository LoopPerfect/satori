#ifndef SATORI_ACTOR_HPP
#define SATORI_ACTOR_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <uv.h>
#include <satori/async.hpp>

namespace satori {

  template<class U>
  struct Actor : Async {
    std::unique_ptr<U> inbox;
    using ValueType = std::remove_reference_t<decltype(*inbox->begin())>;
    using FunctorType = std::function<void(ValueType)>;

    template<class F>
    Actor(uv_loop_t* loop, F const& f)
      : inbox(std::make_unique<U>()), Async(loop, [this, f] {
        for (auto const& msg : inbox) {
          f(msg);
        }
        inbox->clear();
      })
    {}

    void push(ValueType value) {
      inbox->push_back(value);
      invoke();
    }

    ~Actor() {}
  };

}

#endif
