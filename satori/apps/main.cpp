#include <iostream>
#include <assert.h>
#include <memory>
#include <functional>
#include <type_traits>
#include <queue>
#include <tuple>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <satori/satori.hpp>

int main() {

  using namespace std;
  using namespace Satori;

  auto loop = std::make_shared<Loop>();

  using ConType = std::tuple<Tcp*, Tcp*>;

  auto act = std::make_shared<Actor<vector<ConType>>>(loop.get(), [=](ConType const& con) {
    auto server =  std::get<0>(con);
    auto client =  std::get<1>(con);
    client->read();
    client->onData = [=](char const* data, size_t const len) {
      auto req = parseReq(std::string(data, data + len));
      if (req.size() == 0) {
        std::cout << "dasd" << std::endl;
      }

      char res[] =
      "HTTP/1.1 200 OK\r\n"
      "Server: nginx/1.13.5\r\n"
      "Date: Wed, 13 Sep 2017 17:46:27 GMT\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: 11\r\n"
      "Last-Modified: Wed, 13 Sep 2017 17:45:22 GMT\r\n"
      "Connection: keep-alive\r\n"
      "ETag: \"59b96eb2-c\"\r\n"
      "Accept-Ranges: bytes\r\n"
      "\r\n"
      "hello world";

      auto writer = loop->newWrite();
      writer->write(client, res);
      writer->onWriteEnd = [=](int status) {
        if (status<0) { std::cout << status << std::endl; }
        client->close();
        writer->close();
      };
    };

    client->onDataEnd = []{
      std::cout << "data end" << std::endl;
    };

  });

  auto* server = loop->newTcp();
  server->listen("127.0.0.1", 8080);
  server->onListen = [=](auto status) {
    auto* client = loop->newTcp();
    server->accept(client);
    act->push(make_tuple(server, client));
  };

  loop->run();
}
