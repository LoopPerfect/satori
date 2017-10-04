#include <iostream>
#include <memory>
#include <string>

#include <satori/satori.hpp>

int main() {

  using namespace satori;

  auto loop = std::make_shared<Loop>();

  loop->newGetAddrInfo("http://www.google.com/", "80")->onResolved = [=](auto s, auto addr) {

    auto* tcp = loop->newTcp();

    loop->newConnectTcp(tcp, addr)->onConnect = [=](int status) {
      std::cout << "connected " << status << std::endl;

      auto message = "GET / HTTP/1.1\nHost: google.com:80\r\n\r\n";

      loop->newWrite(tcp, message)->onWriteEnd = [](auto...) {
        std::cout << std::endl;
      };

      tcp->read();
      tcp->onData = [=](char const* base, unsigned len) {
        std::cout << base;
        tcp->close();
      };

      tcp->onDataEnd = [=] {
        std::cout << std::endl;
        tcp->close();
      };

    };
  };

  loop->run();

  return 0;
}
