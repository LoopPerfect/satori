#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory>
#include <functional>
#include <type_traits>
#include <queue>
#include <string.h>
#include <unistd.h>

#include <uv.h>


#include <satori/http-parser.hpp>
#include <satori/satori.hpp>
#include <satori/loop.hpp>

int main() {

  using namespace std;
  using namespace Satori;

  auto loop = std::make_shared<Loop>();
  auto* server = loop->takeTcp();

  server->onListen = [=](auto status) {

    auto* client = loop->takeTcp();
    server->accept(client);

    client->onData = [=](char const* data, size_t const len) {
      auto req = parseReq(std::string(data, data+len));
      char res[] =
        "HTTP/1.1 200 OK\r\n"
        "Server: nginx/1.13.5\r\n"
        "Date: Wed, 13 Sep 2017 17:46:27 GMT\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 12\r\n"
        "Last-Modified: Wed, 13 Sep 2017 17:45:22 GMT\r\n"
        "Connection: keep-alive\r\n"
        "ETag: \"59b96eb2-c\"\r\n"
        "Accept-Ranges: bytes\r\n"
        "\r\n"
        "hello world";


      auto writer = loop->takeWriter();
      writer->onWriteEnd = [=](int status) {
        if(status<0) { std::cout << status << std::endl; }
        client->close();
      };
      writer->write(client, res, sizeof(res));

    };

    client->onDataEnd = [=]{
      std::cout << "data end" << std::endl;
    };
    client->read();
  };

  server->listen("127.0.0.1", 8081);
  loop->run();
}
