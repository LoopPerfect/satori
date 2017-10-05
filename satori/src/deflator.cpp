#include <cassert>
#include <memory>
#include <satori/deflator.hpp>
#include <string>
#include <zlib.h>

using namespace satori;

static unsigned int const CHUNK = 16384;

struct Deflator::State {
  z_stream stream;
  std::string result;
};

void Deflator::feed(std::string const& chunk, int const type) {

  unsigned char out[CHUNK];

  state->stream.avail_in = chunk.size();
  state->stream.next_in = (Bytef*)chunk.c_str();

  do {
    state->stream.avail_out = CHUNK;
    state->stream.next_out = out;
    auto result = deflate(&state->stream, type);
    assert(result !=
           Z_STREAM_ERROR); // TODO: Find a better error-handling mechanism
    auto have = CHUNK - state->stream.avail_out;
    for (int i = 0; i < have; ++i) {
      state->result += out[i];
    }
  } while (state->stream.avail_out == 0);
}

Deflator::Deflator() : state(std::make_shared<State>()) {
  state->stream.zalloc = Z_NULL;
  state->stream.zfree = Z_NULL;
  state->stream.opaque = Z_NULL;
  auto result = deflateInit(&state->stream, Z_DEFAULT_COMPRESSION);
  assert(result == Z_OK); // TODO: Find a better error-handling mechanism
}

Deflator::~Deflator() { deflateEnd(&state->stream); }

std::string Deflator::result() const { return state->result; }

void Deflator::feed(std::string const& chunk) { feed(chunk, Z_NO_FLUSH); }

void Deflator::finish() { feed("", Z_FINISH); }
