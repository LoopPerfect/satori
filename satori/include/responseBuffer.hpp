#ifndef SATORI_RESPONSE_BUFFER_HPP
#define SATORI_RESPONSE_BUFFER_HPP

#include <string>
#include <map>

#include <satori/loop.hpp>
#include <satori/httpStatusCode.hpp>

namespace satori {

struct ResponseBuffer {
  Loop* loop;
  Tcp* client;
  std::string buffer = "";

  ResponseBuffer(Loop* loop, Tcp* client)
    : loop{loop}
    , client{client}
  {}

  ResponseBuffer& protocol(std::string const& proto = "HTTP/1.0") {
    return write(proto+" ");
  }

  ResponseBuffer& status(unsigned short const code) {
    return write(httpStatusCode(code));
  }

  ResponseBuffer& set(std::map<std::string, std::string> const& headers) {
    for (auto const& e : headers) {
      write(e.first+": "+ e.second+"\r\n");
    }
    return write("\r\n");
  }

  ResponseBuffer& write(std::string const& str) {
    buffer += str;
    return *this;
  }

  Write* flush() {
    auto w = loop->newWrite(client, buffer);
    buffer = "";
    return w;
  }

  void end(std::string const& str = "") {
    write(str)
      .flush()->onWriteEnd = [client = this->client](auto) {
        client->close();
    };
    client = nullptr;
  }

  ~ResponseBuffer() {
    if (client) end();
  }

};

}

#endif
