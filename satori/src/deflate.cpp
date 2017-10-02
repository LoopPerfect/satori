#include <string>
#include <cassert>
#include <zlib.h>
#include <satori/deflate.hpp>
#include <satori/zlibError.hpp>

namespace satori {

  std::string deflate(std::string const& uncompressed) {

    int const level = Z_DEFAULT_COMPRESSION;
    unsigned int const CHUNK = 1024; // Magic value is 16384

    int ret;
    unsigned int have;
    z_stream strm;
    unsigned char out[CHUNK];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    ret = deflateInit(&strm, level);

    assert(ret == Z_OK);

    auto result = std::string("");

    strm.avail_in = uncompressed.size();
    strm.next_in = (Bytef*)uncompressed.c_str();

    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = deflate(&strm, Z_FINISH);
      assert(ret != Z_STREAM_ERROR);
      have = CHUNK - strm.avail_out;
      for (int i = 0; i < have; ++i) {
        result += out[i];
      }
    } while (strm.avail_out == 0);

    assert(strm.avail_in == 0);
    assert(ret == Z_STREAM_END);

    deflateEnd(&strm);

    return result;
  }

}
