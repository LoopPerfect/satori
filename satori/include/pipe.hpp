#ifndef SATORI_PIPE_HPP
#define SATORI_PIPE_HPP

#include <uv.h>
#include <satori/stream.hpp>

namespace satori {

  namespace detail {

    template<class T = uv_pipe_t>
    struct Pipe : Stream<T> {
      Pipe(uv_loop_t* loop, bool ipc = false)
        : Stream<T>(loop) {
        uv_pipe_init((uv_loop_t*)loop, (uv_pipe_t*)this, ipc);
      }

      int bindToFile(uv_file file) {
        return uv_pipe_open((uv_pipe_t*)this, file);
      }

      int bindToName(const char* name) {
        return uv_pipe_bind((uv_pipe_t*)this, name);
      }

      ~Pipe() {}
    };

  }

  using Pipe = detail::Pipe<>;
}

#endif
