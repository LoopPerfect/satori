#ifndef SATORI_LOOP_HPP
#define SATORI_LOOP_HPP

#include <uv.h>
#include <satori/handles.hpp>
#include <satori/requests.hpp>
#include <satori/recycler.hpp>

namespace satori {

  struct Loop : uv_loop_t {

    SmartRecycler<1024> pool;

    Loop(size_t const& num = 1024)
      : pool(num) {
      uv_loop_init(this);
    }

    Handle* newHandle() {
      return pool.create<Handle>(this);
    }

    Stream* newStream() {
      return pool.create<Stream>(this);
    }

    Tcp* newTcp() {
      return pool.create<Tcp>(this);
    }

    Write* newWrite() {
      return pool.create<Write>(this);
    }

    template<class F>
    Async* newAsync(F const& f) {
      return pool.create<Async>(this, f);
    }

    FS* newFS() {
      return pool.create<FS>(this);
    }

    Pipe* newPipe(bool ipc = 0) {
      return pool.create<Pipe>(this, ipc);
    }

    Connect* newConnect() {
      return pool.create<Connect>(this);
    }

    GetAddrInfo* newGetAddrInfo() {
      return pool.create<GetAddrInfo>(this);
    }

    Process* newProcess() {
      return pool.create<Process>(this);
    }

    template <typename T>
    Actor<T>* newActor() {
      return pool.create<Actor<T>>(this);
    }

    /*
    Work newWork() {
      return new (pool.aquire()) Work(this);
    }
    */

    uint64_t now() {
      return uv_now(this);
    }

    int run(uv_run_mode mode = UV_RUN_DEFAULT) {
      return uv_run(this, mode);
    }

    ~Loop() {
      uv_loop_close(this);
    }
  };

  void release(Loop* loop, void* ptr);
}

#endif
