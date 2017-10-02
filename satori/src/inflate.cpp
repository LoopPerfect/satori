#include <string>
#include <cassert>
#include <zlib.h>
#include <satori/inflate.hpp>

namespace satori {

  std::string inflate(std::string const& compressed) {

    unsigned int const CHUNK = 1024; // Magic value is 16384

    int ret;
    unsigned have;
    z_stream strm;

    unsigned char out[CHUNK];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    ret = inflateInit(&strm);

    assert(ret == Z_OK);

    auto result = std::string("");

    do {
      strm.avail_in = compressed.size();
      if (strm.avail_in == 0) {
        break;
      }
      strm.next_in = (Bytef*)compressed.c_str();
      do {
        strm.avail_out = CHUNK;
        strm.next_out = out;
        ret = inflate(&strm, Z_NO_FLUSH);
        assert(ret != Z_STREAM_ERROR);
        switch (ret) {
          case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
          case Z_DATA_ERROR:
          case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            break;
        }
        have = CHUNK - strm.avail_out;
        for (int i = 0; i < have; ++i) {
          result += out[i];
        }
      } while (strm.avail_out == 0);
    } while (ret != Z_STREAM_END);
    inflateEnd(&strm);
    return result;
  }

}
