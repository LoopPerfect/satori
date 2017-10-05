#include <cassert>
#include <satori/inflate.hpp>
#include <satori/inflator.hpp>
#include <string>
#include <zlib.h>

namespace satori {

std::string inflate(std::string const& compressed) {

  auto inflator = Inflator();

  inflator.feed(compressed);

  return inflator.result();
}

} // namespace satori
