#ifndef SATORI_LOOP_HPP
#define SATORI_LOOP_HPP

#include <string>
#include <uv.h>

#include <satori/actor.hpp>
#include <satori/fs.hpp>
#include <satori/handles.hpp>
#include <satori/managed_ptr.hpp>
#include <satori/recycler.hpp>
#include <satori/requests.hpp>
#include <satori/runMode.hpp>

namespace satori {

struct Loop : uv_loop_t {

  SmartRecycler<1024> pool;

  Loop(size_t const &num = 1024) : pool(num) { uv_loop_init(this); }

  managed_ptr<Tcp> newTcp() {
    return managed_ptr<Tcp>(pool.create<Tcp>(this));
  }

  Async *newAsync() { return pool.create<Async>(this); }

  Pipe *newPipe(bool ipc = 0) { return pool.create<Pipe>(this, ipc); }

  template <class S>
  Write *newWrite(S *stream, std::string const &msg) {
    return pool.create<Write>((uv_stream_t *)stream, msg);
  }

  template <class P> ConnectPipe *newConnectPipe(P *pipe, const char *name) {
    return pool.create<ConnectPipe>((uv_pipe_t *)pipe, name);
  }

  template <class T>
  managed_ptr<ConnectTcp> newConnectTcp(T *tcp, addrinfo const &addr) {
    return managed_ptr<ConnectTcp>(pool.create<ConnectTcp>((uv_tcp_t *)tcp, addr));
  }

  managed_ptr<GetAddrInfo> newGetAddrInfo(std::string const &host,
                                          std::string const &port) {
    return managed_ptr<GetAddrInfo>(pool.create<GetAddrInfo>(this, host, port));
  }

  template <class... Xs> managed_ptr<FSOpen> newFSOpen(Xs... xs) {
    return managed_ptr<FSOpen>(pool.create<FSOpen>(this, xs...));
  }

  template <class... Xs> managed_ptr<FSClose> newFSClose(Xs... xs) {
    return managed_ptr<FSClose>(pool.create<FSClose>(this, xs...));
  }

  template <class S> FSWrite *newFSWrite(S *s, std::string &msg) {
    return pool.create<FSWrite>(this, s, msg);
  }

  template <class... Xs> managed_ptr<FSRead> newFSRead(Xs... xs) {
    return managed_ptr<FSRead>(pool.create<FSRead>(this, xs...));
  }

  template <class... Xs> FSStat *newFSStat(Xs... xs) {
    return pool.create<FSStat>(this, xs...);
  }

  template <class... Xs>
  managed_ptr<FSMkdir> newFSMkdir(Xs... xs) {
    return managed_ptr<FSMkdir>(pool.create<FSMkdir>(this, xs...));
  }

  template <class... Xs>
  managed_ptr<FSScanDir> newFSScanDir(Xs... xs) {
    return managed_ptr<FSScanDir>(pool.create<FSScanDir>(this, xs...));
  }

  template <class... Xs>
  managed_ptr<FSUTime> newFSUTime(Xs... xs) {
    return managed_ptr<FSUTime>(pool.create<FSUTime>(this, xs...));
  }

  template <class... Xs>
  managed_ptr<FSRealPath> newFSRealPath(Xs... xs) {
    return managed_ptr<FSRealPath>(pool.create<FSRealPath>(this, xs...));
  }

  template <class... Xs> managed_ptr<FSUnlink> newFSUnlink(Xs... xs) {
    return managed_ptr<FSUnlink>(pool.create<FSUnlink>(this, xs...));
  }

  template <class... Xs> managed_ptr<FSPoll> newFSPoll(Xs... xs) {
    return managed_ptr<FSPoll>(pool.create<FSPoll>(this, xs...));
  }

  template <typename T> Actor<T> *newActor() {
    return pool.create<Actor<T>>(this);
  }

  /*
  Process* newProcess() {
    return pool.create<Process>(this);
  }

  Work newWork() {
    return new (pool.aquire()) Work(this);
  }
  */

  /**
   * Forwards to uv_update_time.
   *
   * From the libuv docs:
   *  Update the event loop’s concept of “now”. Libuv caches the current time
   *  at the start of the event loop tick in order to reduce the number of
   *  time-related system calls.
   */
  void updateTime() {
    uv_update_time(this);
  }

  uint64_t now() { return uv_now(this); }

  int run(RunMode const mode = RunMode::Default) {

    auto const toUVRunMode = [](RunMode const mode) -> uv_run_mode {
      switch (mode) {
      default:
      case satori::RunMode::Default:
        return UV_RUN_DEFAULT;
      case satori::RunMode::Once:
        return UV_RUN_ONCE;
      case satori::RunMode::NoWait:
        return UV_RUN_NOWAIT;
      }
    };

    return uv_run(this, toUVRunMode(mode));
  }

  ~Loop() { uv_loop_close(this); }
};

template <class H> void release(H h) { ((Loop *)h->loop)->pool.destroy(h); }

template <class R> void releaseRequest(R r) {
  ((Loop *)r->handle->loop)->pool.destroy(r);
  void release(Loop * loop, void *ptr);
}

} // namespace satori

#endif
