#include <iostream>
#include <memory>
#include <string>
#include <functional>

#include <satori/satori.hpp>

int main() {

  using namespace satori;

  auto loop = std::make_shared<Loop>();

  auto getAddrInfo = loop->newGetAddrInfo("google.com", "80");

  getAddrInfo->onResolved = [=](auto status, auto addr) {

    if (status < 0) {
      std::cerr << "Non-zero status (" << status << ")" << std::endl;
      exit(1);
    }

    auto* tcp = loop->newTcp();

    loop->newConnectTcp(tcp, addr)->onConnect = [=](int status) {

      auto message = "GET / HTTP/1.1\nHost: google.com:80\r\n\r\n";

      loop->newWrite(tcp, message)->onWriteEnd = [](auto...) {
        std::cout << std::endl;
      };

      tcp->onData = [=](int const result, StringView const& data) {
        // A negative result is an error
        if (result < 0) {
          std::cerr << errorName(result) << " " << errorMessage(result) << std::endl;
          exit(1);
        }
        std::cout << data;
      };

      tcp->onDataEnd = [=]() {
        std::cout << std::endl;
        tcp->close();
      };

      tcp->read();

    };
  };

  loop->run();

  return 0;
}
