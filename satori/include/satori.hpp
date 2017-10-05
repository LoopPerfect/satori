#ifndef SATORI_SATORI_HPP
#define SATORI_SATORI_HPP

#include <memory>
#include <uv.h>

#include <satori/actor.hpp>
#include <satori/enableMultiProcess.hpp>
#include <satori/errors.hpp>
#include <satori/directoryEntry.hpp>
#include <satori/handles.hpp>
#include <satori/httpParser.hpp>
#include <satori/loop.hpp>
#include <satori/promise.hpp>
#include <satori/recycler.hpp>
#include <satori/requests.hpp>
#include <satori/router.hpp>
#include <satori/url.hpp>
#include <satori/url-parser.hpp>
#include <satori/url.hpp>
#include <satori/urlEncode.hpp>
#include <satori/urlDecode.hpp>
#include <satori/inflate.hpp>
#include <satori/inflator.hpp>
#include <satori/deflate.hpp>
#include <satori/deflator.hpp>
#include <satori/managed_ptr.hpp>

namespace satori {

  struct Satori {

  private:

    std::shared_ptr<Loop> loop;

  public:

    Satori() : loop(std::make_shared<Loop>()) {}

    ~Satori() {}

    template <typename T>
    Promise<T> promise() {
      return Promise<T>(std::weak_ptr<Loop>(loop));
    }

    void run(uv_run_mode mode = UV_RUN_DEFAULT) {
      loop->run(mode);
    }

  };

}

#endif
