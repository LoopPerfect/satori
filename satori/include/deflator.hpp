#ifndef SATORI_DEFLATOR_HPP
#define SATORI_DEFLATOR_HPP

#include <string>
#include <memory>
#include <zlib.h>

namespace satori {

  struct Deflator {

private:

    unsigned int const CHUNK = 16384;

    struct State {
      z_stream stream;
      std::string result;
    };

    std::shared_ptr<State> state;

    void feed(std::string const& chunk, int const type) {

      unsigned char out[CHUNK];

      state->stream.avail_in = chunk.size();
      state->stream.next_in = (Bytef*)chunk.c_str();

      do {
        state->stream.avail_out = CHUNK;
        state->stream.next_out = out;
        auto result = deflate(&state->stream, type);
        assert(result != Z_STREAM_ERROR); // TODO: Find a better error-handling mechanism
        auto have = CHUNK - state->stream.avail_out;
        for (int i = 0; i < have; ++i) {
          state->result += out[i];
        }
      } while (state->stream.avail_out == 0);
    }

public:

    Deflator() : state(std::make_shared<State>()) {
      state->stream.zalloc = Z_NULL;
      state->stream.zfree = Z_NULL;
      state->stream.opaque = Z_NULL;
      auto result = deflateInit(&state->stream, Z_DEFAULT_COMPRESSION);
      assert(result == Z_OK); // TODO: Find a better error-handling mechanism
    }

    ~Deflator() {
      deflateEnd(&state->stream);
    }

    std::string result() const {
      return state->result;
    }

    void feed(std::string const& chunk) {
      feed(chunk, Z_NO_FLUSH);
    }

    void finish() {
      feed("", Z_FINISH);
    }

  };

}

#endif
