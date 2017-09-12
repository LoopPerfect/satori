#ifndef SATORI_LOOP_HPP
#define SATORI_LOOP_HPP

namespace Satori {

  struct Loop {

    uv_loop_t* loop;

    Loop(uv_loop_t* loop)
      : loop(loop) {
      uv_loop_init(loop);
    }

    void run(uv_run_mode mode) {
      uv_run(loop, mode);
    }

    ~Loop() {
      uv_loop_close(loop);
    }
  };
}

#endif
