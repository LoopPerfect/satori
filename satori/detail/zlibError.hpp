#ifndef SATORI_ZLIB_ERROR
#define SATORI_ZLIB_ERROR

#include <string>
#include <zlib.h>

namespace satori {

  std::string zlibError(int const code) {
    switch (code) {
      case Z_STREAM_ERROR:
        return "Invalid compression level";
      case Z_DATA_ERROR:
        return "invalid or incomplete deflate data";
      case Z_MEM_ERROR:
        return "out of memory";
      case Z_VERSION_ERROR:
        return "zlib version mismatch";
      case Z_OK:
        return "OK";
    }
    return "Unknown error";
  }

}

#endif
