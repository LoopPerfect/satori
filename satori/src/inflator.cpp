#include <cassert>
#include <memory>
#include <satori/inflator.hpp>
#include <string>
#include <zlib.h>

using namespace satori;

static unsigned int const CHUNK = 16384;

struct Inflator::State {
  z_stream stream;
  bool finished;
  std::string result;
};

Inflator::Inflator() : state(std::make_shared<State>()) {
  state->stream.zalloc = Z_NULL;
  state->stream.zfree = Z_NULL;
  state->stream.opaque = Z_NULL;
  state->stream.avail_in = 0;
  state->stream.next_in = Z_NULL;
  auto result = inflateInit(&state->stream);
  assert(result == Z_OK); // TODO: Find a better error-handling mechanism
}

Inflator::~Inflator() { inflateEnd(&state->stream); }

std::string Inflator::result() const {
  assert(state->finished && "This stream has not yet been finished");
  return state->result;
}

bool Inflator::feed(std::string const& chunk) {
  assert(!state->finished && "This stream has already been finished");
  int ret;
  unsigned char out[CHUNK];
  state->stream.avail_in = chunk.size();
  if (state->stream.avail_in == 0) {
    return false;
  }
  state->stream.next_in = (Bytef*)chunk.c_str();
  do {
    state->stream.avail_out = CHUNK;
    state->stream.next_out = out;
    ret = inflate(&state->stream, Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);
    switch (ret) {
    case Z_NEED_DICT:
      ret = Z_DATA_ERROR;
    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
      state->finished = true;
      inflateEnd(&state->stream);
      return true;
    }
    auto have = CHUNK - state->stream.avail_out;
    for (int i = 0; i < have; ++i) {
      state->result += out[i];
    }
  } while (state->stream.avail_out == 0);
  bool finished = ret == Z_STREAM_END;
  if (finished) {
    state->finished = finished;
    inflateEnd(&state->stream);
  }
  return finished;
}
