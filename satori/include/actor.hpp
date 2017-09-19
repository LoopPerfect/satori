#ifndef SATORI_ACTOR_HPP
#define SATORI_ACTOR_HPP

#include <functional>
#include <type_traits>

#include <satori/handles.hpp>

namespace Satori {

template<class U>
struct Actor : Async {
  U inbox;
  using ValueType = std::remove_reference_t<decltype(*inbox.begin())>;
  using FunctorType = std::function<void(ValueType)>;

  template<class F>
  Actor(void* loop, F const& f)
    : Async(loop, [this, f] {
      for (auto const& msg : inbox) {
        f(msg);
      }
      inbox.clear();
    })
  {}

  void push(ValueType value) {
    inbox.push_back(value);
    invoke();
  }

  ~Actor() {}
};

}

#endif
