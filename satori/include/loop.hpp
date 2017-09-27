#ifndef SATORI_LOOP_HPP
#define SATORI_LOOP_HPP

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

    Tcp* newTcp() {
      return pool.create<Tcp>(this);
    }

    template<class S>
    Write* newWrite(S* stream, std::string const& msg) {
      return pool.create<Write>((uv_stream_t*)stream, msg);
    }

    template<class F>
    Async* newAsync(F const& f) {
      return pool.create<Async>(this, f);
    }



    Pipe* newPipe(bool ipc = 0) {
      return pool.create<Pipe>(this, ipc);
    }
/*
    FS* newFS() {
      return pool.create<FS>(this);
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


  template<class H>
  void release(H h) {
    ((Loop*)h->loop)->pool.release(h);
  }

  template<class R>
  void releaseRequest(R r) {
    ((Loop*)r->handle->loop)->pool.release(r);
  }

}

#endif
