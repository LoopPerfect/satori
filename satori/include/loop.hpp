#ifndef SATORI_LOOP_HPP
#define SATORI_LOOP_HPP

#include <satori/handles.hpp>
#include <satori/requests.hpp>
#include <satori/recycler.hpp>

namespace satori {

  // TODO: replace with a variant...
  struct AnyHandle {
    union {
      char raw[0];
      Handle handle;
      Stream stream;
      Tcp tcp;
      Write write;
      Async async;
      FS fs;
      Pipe pipe;
      Connect connect;
      GetAddrInfo getAddrInfo;
      Process process;
    };

    int type = 0;

    AnyHandle(int type = 0)
      : type{type} {
    }

    ~AnyHandle() {
      switch (type) {
        case 1:
          handle.~Handle();
          break;
        case 2:
          stream.~Stream();
          break;
        case 3:
          tcp.~Tcp();
          break;
        case 4:
          write.~Write();
          break;
        case 5:
          async.~Async();
          break;
        case 6:
          fs.~FS();
          break;
        case 7:
          pipe.~Pipe();
          break;
        case 8:
          connect.~Connect();
          break;
        case 9:
          getAddrInfo.~GetAddrInfo();
          break;
        case 10:
          process.~Process();
          break;
        case 0:
        default:
          break;
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

    FS* newFS() {
      return new (pool.acquire(6)) FS(this);
    }

    Pipe* newPipe(bool ipc = 0) {
      return new (pool.acquire(7)) Pipe(this, ipc);
    }

    Connect* newConnect() {
      return new (pool.acquire(8)) Connect(this);
    }

    GetAddrInfo* newGetAddrInfo() {
      return new (pool.acquire(9)) GetAddrInfo(this);
    }

    Process* newProcess() {
      return new (pool.acquire(10)) Process(this);
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
