#include <cassert>
#include <satori/deflate.hpp>
#include <satori/deflator.hpp>
#include <string>
#include <zlib.h>

namespace satori {

std::string deflate(std::string const& uncompressed) {

  auto deflator = Deflator();

  deflator.feed(uncompressed);
  deflator.finish();

  return deflator.result();
}

} // namespace satori
