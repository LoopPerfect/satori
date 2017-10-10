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

  auto loop = std::make_shared<Loop>();

  using ConType = std::tuple<Tcp *, Tcp *>;

  auto act = [=](ConType const &con) {
    auto server = std::get<0>(con);
    auto client = std::get<1>(con);

    auto parser = std::make_shared<satori::HttpParser>(HTTP_REQUEST);

    client->read();
    client->onData = [=](char const *data, size_t const len) {
      parser->execute(std::string(data, len));
      client->stop();
    };

    parser->onUrl = [=](http_parser const *parser, char const *at,
                        size_t length) {
      auto path = std::string(at, length);
      auto file = "/home/gaetano/Projects/satori" + path;

      loop->newFSOpen(file, O_RDONLY, S_IRUSR)->onOpen = [=](auto fid) {
        if (fid < 0) {
          loop->newWrite(client, "error reading file")->onWriteEnd =
              [=](int s) {
                std::cout << "to many open files " << s << std::endl;
                client->close();
                loop->newFSClose(fid);
              };
          return;
        }
        loop->newFSRead(fid)->onRead = [=](int bytes, auto buf) {
          if (bytes >= 0) {
            auto chunk = std::string(buf.base, buf.len);
            auto write = loop->newWrite(client, chunk);

            if (bytes <= buf.len) {
              write->onWriteEnd = [=](int) {
                client->close();
                loop->newFSClose(fid);
              };
            }

          } else {
            std::cout << "read error" << std::endl;
            client->close();
          }
        };
      };
      return 1;
    };

  };

  auto *server = loop->newTcp();
  server->listen("127.0.0.1", 8080);
  server->onListen = [=](auto status) {
    if (status < 0) {
      std::cout << "flaky client" << std::endl;
      return;
    }
    auto *client = loop->newTcp();
    server->accept(client);
    act(make_tuple(server, client));
  };

  loop->run();
}
