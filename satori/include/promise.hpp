#ifndef SATORI_PROMISE_HPP
#define SATORI_PROMISE_HPP

#include <vector>
#include <functional>
#include <cassert>

template <typename T>
struct Promise {

  union {
    T value;
  };

  // Actor<std::function<T>>* actor;
  bool isDone = false;
  std::vector<std::function<void(T)>> callbacks = {};

  Promise() :
    isDone(false),
    callbacks({}) {}

  ~Promise() {
    if (isDone) {
      value.~T();
    }
  }

  // template <typename R>
  // Promise<R> map(Loop& loop, std::function<R(T)> f) {
  //   if (isDone) {
  //     return f(value);
  //   }
  //   auto result = Promise<R>;
  //   actor->push([this, result]() {
  //     assert(isDone && "Callback called too soon");
  //     result.resolve(f(value));
  //   });
  //   return result;
  // }

  // template <typename F>
  // Promise flatMap(F f) {
  //
  // }

  // template <typename F>
  // Promise then() {
  //
  // }

  void onResolve(std::function<void(T)> f) {
    if (isDone) {
      f(value);
    } else {
      callbacks.push_back(f);
    }
  }

  void resolve(T result) {
    assert(!isDone && "Cannot set the result twice");
    isDone = true;
    value = result;
    for (auto const& callback : callbacks) {
      callback(value);
    }
    callbacks.clear();
  }
};

#endif
