#ifndef SATORI_RECYCLER_HPP
#define SATORI_RECYCLER_HPP

#include <deque>
#include <stack>

namespace Satori {

  template<class T>
  struct Recycler {
    std::deque<T> store;
    std::stack<T*> pool;

    Recycler(size_t const num)
      : store(num) {
      for(auto& x: store) {
        pool.push(&x);
      }
    }

    T* take() {
      if (pool.size() == 0) {
        auto const n = store.size();
        store.emplace_back();
        return &store[n];
      }
      auto g = pool.top();
      pool.pop();
      return g;
    }

    void release(T* o) {
      o->~T();
      pool.push(o);
    }
  };

}

#endif
