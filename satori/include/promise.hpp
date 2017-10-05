#ifndef SATORI_PROMISE_HPP
#define SATORI_PROMISE_HPP

#include <cassert>
#include <functional>
#include <memory>
#include <vector>

namespace satori {

template <typename T>
struct Promise;

template <typename T>
constexpr auto ensurePromiseType(Promise<T> const& p) {
  return p;
}

template <typename P>
constexpr auto ensurePromise(P const& p)
  -> decltype(ensurePromiseType<typename P::ValueType>(p)) {
  return p;
}

struct Loop;

template <typename T>
struct Promise {

private:
  struct State {
    union {
      T value;
    };
    bool isDone = false;
    std::vector<std::function<void(T)>> callbacks;

    State() {}

    State(State const& rhs) = delete;

    ~State() {
      if (isDone) {
        value.~T();
        isDone = false;
      }
    }

    void setValue(T const& x) {
      assert(!isDone && "Cannot set value more than once");
      new (&value) T(x);
      isDone = true;
    }
  };

  mutable std::shared_ptr<State> state;

public:
  using ValueType = T;

  std::weak_ptr<Loop> loop;

  Promise(std::weak_ptr<Loop> loop)
    : loop(loop), state(std::make_shared<State>()) {}

  Promise(Promise const& rhs) = default;

  ~Promise() = default;

  void executeCallback(T result, std::function<void(T)> callback) const {
    auto l = loop.lock();
    assert(l && "loop has already been destroyed");
    auto* async = l->newAsync();
    async->job = [=]() {
      callback(result);
      async->close();
    };
    async->invoke();
  }

  void resolve(T result) {
    state->setValue(result);
    for (auto const& callback : state->callbacks) {
      executeCallback(result, callback);
    }
    state->callbacks.clear();
  }

  void onResolve(std::function<void(T)> callback) const {
    if (state->isDone) {
      executeCallback(state->value, callback);
    } else {
      state->callbacks.push_back(callback);
    }
  }

  template <typename F>
  auto flatMap(F f) const -> decltype(ensurePromise(f(this->state->value))) {
    using R = decltype(f(this->state->value));
    auto p = R(loop);
    onResolve([=](T result) mutable {
      auto q = f(result);
      q.onResolve([p](auto x) mutable { p.resolve(x); });
    });
    return p;
  }

  template <typename F>
  auto map(F f) const -> Promise<decltype(f(this->state->value))> {
    using R = decltype(f(this->state->value));
    return flatMap([this, f](auto x) mutable {
      auto p = Promise<R>(this->loop);
      p.resolve(f(x));
      return p;
    });
  }
};

} // namespace satori

#endif
