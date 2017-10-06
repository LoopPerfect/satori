#include <assert.h>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <tuple>
#include <type_traits>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <satori/satori.hpp>

int main() {

  using namespace std;
  using namespace satori;

  static auto loop = std::make_shared<Loop>();
  static std::string const res =
    "HTTP/1.1 200 OK\r\n"
    //"Server: nginx/1.13.5\r\n"
    "Date: Wed, 13 Sep 2017 17:46:27 GMT\r\n"
    //"Content-Type: text/html\r\n"
    "Content-Length: 11\r\n"
    //"Last-Modified: Wed, 13 Sep 2017 17:45:22 GMT\r\n"
    //"Connection: keep-alive\r\n"
    //"ETag: \"59b96eb2-c\"\r\n"
    //"Accept-Ranges: bytes\r\n"
    "\r\n"
    "hello world";


  static auto* server = loop->newTcp();
  server->listen("127.0.0.1", 8080);
  int i=0;
  server->onListen = [=](auto status) mutable {
    auto* client = loop->newTcp();
    server->accept(client);

    //std::cout << "opening: " << i << std::endl;
    client->read();

    loop->newWrite(client, res)
      ->onWriteEnd = [i](int) {
      //std::cout << "writeEnd: " << i << std::endl;
    };

    client->onData = [client, i](int status, StringView sv) {
      //std::cout << "closing: " << i << std::endl;
      client->close();
    };
    ++i;
  };

  loop->run();
}
