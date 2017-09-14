#ifndef SATORI_LOOP_HPP
#define SATORI_LOOP_HPP

#include <satori/god-recycler.hpp>
namespace Satori {
  struct Loop {

    GodRecycler gr;
    uv_loop_t* loop;

    Write* takeWriter() {
      auto* g = gr.take();
      g->initAsWriter(loop);
      return &g->cb.write;
    }

    Tcp* takeTcp() {
      auto* g = gr.take();
      g->initAsTcp(loop);
      return &g->cb.tcp;
    }

    Loop(size_t n = 1024, uv_loop_t* loop = uv_default_loop())
      : gr(n)
      , loop(loop) {
      uv_loop_init(loop);
    }

    void run(uv_run_mode mode = UV_RUN_DEFAULT) {
      uv_run(loop, mode);
    }

    ~Loop() {
      uv_loop_close(loop);
    }
  };
}

#endif
