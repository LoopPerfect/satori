#include <string>
#include <cassert>
#include <zlib.h>
#include <satori/inflate.hpp>
#include <satori/inflator.hpp>

namespace satori {

  std::string inflate(std::string const& compressed) {

    auto inflator = Inflator();

    inflator.feed(compressed);

    return inflator.result();
  }

}
