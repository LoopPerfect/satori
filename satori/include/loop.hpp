#ifndef SATORI_LOOP_HPP
#define SATORI_LOOP_HPP

#include <satori/handles.hpp>
#include <satori/requests.hpp>
#include <satori/recycler.hpp>

namespace Satori {

  // TODO: replace with a variant...
  struct AnyHandle {
    union {
      char raw[0];
      Handle handle;
      Stream stream;
      Tcp tcp;
      Write write;
      Async async;
      Pipe pipe;
    };

    int type = 0;

    AnyHandle(int type = 0)
      : type{type}
    {}

    ~AnyHandle() {
      switch (type) {
        default:
        case 0: break;
        case 1: handle.~Handle(); break;
        case 2: stream.~Stream(); break;
        case 3: tcp.~Tcp(); break;
        case 4: write.~Write(); break;
        case 5: async.~Async(); break;
        case 6: pipe.~Pipe(); break;
      }
    }
  };


  struct Loop : uv_loop_t {

    Recycler<AnyHandle> pool;

    Loop(size_t const& num = 1024)
      : pool(num) {
      uv_loop_init(this);
    }

    Handle* newHandle() {
      return new (pool.acquire(1)) Handle(this);
    }

    Stream* newStream() {
      return new (pool.acquire(2)) Stream(this);
    }

    Tcp* newTcp() {
      return new (pool.acquire(3)) Tcp(this);
    }

    Write* newWrite() {
      return new (pool.acquire(4)) Write(this);
    }

    template<class F>
    Async* newAsync(F const& f) {
      return new (pool.acquire(5)) Async(this, f);
    }

    Pipe* newPipe(bool ipc=0) {
      return new (pool.acquire(6)) Pipe(this, ipc);
    }


    /*
    Work newWork() {
      return new (pool.aquire()) Work(this);
    }
    */

    void run(uv_run_mode mode = UV_RUN_DEFAULT) {
      uv_run(this, mode);
    }

    ~Loop() {
      uv_loop_close(this);
    }
  };

  void release(Loop* loop, void* ptr);

}

#endif
