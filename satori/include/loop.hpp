#ifndef SATORI_LOOP_HPP
#define SATORI_LOOP_HPP

#include <satori/handles.hpp>
#include <satori/requests.hpp>

namespace Satori {

  struct Loop : uv_loop_t {
    Loop() {
      uv_loop_init(this);
    }

    Handle* newHandle() {
      return new Handle(this);
    }

    Stream* newStream() {
      return new Stream(this);
    }

    Tcp* newTcp() {
      return new Tcp(this);
    }

    Write* newWrite() {
      return new Write(this);
    }

    Async* newAsync() {
      return new Async(this);
    }

    Work newWork() {
      return new Work(this);
    }


    template<class T>
    void release(T*ptr) {
      delete ptr;
    }

    void run(uv_run_mode mode = UV_RUN_DEFAULT) {
      uv_run(this, mode);
    }

    ~Loop() {
      uv_loop_close(this);
    }
  };

}

#endif
