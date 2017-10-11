#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include <satori/satori.hpp>

int main() {

  using namespace satori;

  auto loop = std::make_shared<Loop>();

  auto getAddrInfo = loop->newGetAddrInfo("google.com", "80");

  getAddrInfo->onResolved = [=](int const status, std::vector<uv_addrinfo> const& addrs) {

    if (status < 0) {
      std::cerr << "Non-zero status (" << status << ")" << std::endl;
      exit(1);
      return;
    }

    if (addrs.empty()) {
      std::cerr << "Could not resolve address. " << std::endl;
      exit(1);
      return;
    }

    auto addr = addrs[0];

    auto tcp = loop->newTcp();

    loop->newConnectTcp(tcp.get(), addr)->onConnect = [=](int const status) mutable {

      auto message = std::string("GET / HTTP/1.1\nHost: google.com:80\r\n\r\n");

      loop->newWrite(tcp.get(), message)->onWriteEnd = [](auto...) {
        std::cout << std::endl;
      };

      tcp->onData = [=](int const result, StringView const& data) mutable {
        // A negative result is an error
        if (result < 0) {
          std::cerr << errorName(result) << " " << errorMessage(result) << std::endl;
          exit(1);
        }
        std::cout << data;
      };

      tcp->onDataEnd = [=]() mutable {
        std::cout << std::endl;
        tcp->close();
      };

      tcp->read();
    };
  };

  loop->run();

  return 0;
}
