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

    template<class U=T, class...Xs>
    U* create(Xs&&...xs) {
      if (pool.size() == 0) {
        auto const n = store.size();
        store.resize(n+1);
        return new (&store[n])(xs...);
      }
      U* g = pool.top();
      pool.pop();
      return g;
    }

    template<class U=T>
    void release(U* o) {
      o->~U();
      pool.push((T*)o);
    }
  };

}

#endif
