#ifndef SATORI_HTTP_RESPONSE_HPP
#define SATORI_HTTP_RESPONSE_HPP

#include <map>
#include <memory>
#include <satori/loop.hpp>
#include <satori/responseBuffer.hpp>
#include <string>

namespace satori {

struct HttpResponse {
  std::shared_ptr<ResponseBuffer> buffer;

  std::map<std::string, std::string> headers = {};
  unsigned short statusCode = 200;
  bool headersSent = false;
  std::string protocol = "HTTP/1.0";

  HttpResponse(Loop *loop, Tcp *tcp)
      : buffer(std::make_shared<ResponseBuffer>(loop, tcp)) {}

  HttpResponse(HttpResponse const &r) : buffer(r.buffer) {}

  HttpResponse(HttpResponse &&r) : buffer(std::move(r.buffer)) {}

  HttpResponse() = delete;
  ~HttpResponse() {}

  HttpResponse http(std::string const &str) {
    protocol = "HTTP/" + str;
    return *this;
  }

  HttpResponse &status(unsigned short const status) {
    statusCode = status;
    return *this;
  }

  HttpResponse &set(std::string const &key, std::string const &value) {
    headers[key] = value;
    return *this;
  }

  HttpResponse &set(std::map<std::string, std::string> const &hdrs) {
    for (auto const &e : hdrs) {
      set(e.first, e.second);
    }
    return *this;
  }

  HttpResponse &write(std::string const &str) {
    assert(buffer && "write after close");
    if (!headersSent) {
      buffer->protocol(protocol).status(statusCode).set(headers);
      headersSent = true;
    }

    buffer->write(str);
    return flush();
  }

  HttpResponse &flush() {
    buffer->flush();
    return *this;
  }

  HttpResponse &end(std::string const &str = "") {
    buffer->end(str);
    return *this;
  }

  void close() { buffer.reset(); }
};

} // namespace satori

#endif
