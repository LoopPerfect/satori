#include <string>
#include <cassert>
#include <zlib.h>
#include <satori/deflate.hpp>
#include <satori/deflator.hpp>
#include <satori/zlibError.hpp>

namespace satori {

  std::string deflate(std::string const& uncompressed) {

    auto deflator = Deflator();

    deflator.feed(uncompressed);
    deflator.finish();

    return deflator.result();
  }

}
