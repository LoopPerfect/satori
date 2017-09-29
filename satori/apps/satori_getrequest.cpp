#include <iostream>
#include <memory>
#include <string>

#include <satori/satori.hpp>
#include <uv.h>

std::string error_to_string(int error) {
  return std::string(uv_err_name(error)) + " " +
         std::string(uv_strerror(error));
}

int main(int argc, const char** argv) {

  using namespace satori;

  if (!argv[1]) {
    std::cout << "Usage: host (without protocoll) " << std::endl;
    return 1;
  }

  auto loop = std::make_shared<Loop>();

  loop->newGetAddrInfo(argv[1], "80")->onResolved = [=](auto s, auto addr) {
    std::cout << "resolved " << std::endl;

    auto tcp = loop->newTcp();
    loop->newConnectTcp(tcp, addr)->onConnect = [=](int status) {
      std::cout << "connected " << status << std::endl;

      loop->newWrite(tcp, "GET / HTTP/1.1\nHost: google.com:80\r\n\r\n")
        ->onWriteEnd = [](auto...) { std::cout << "write end" << std::endl; };

      tcp->read();
      tcp->onData = [=](char const* base, unsigned len) {
        std::cout << base << std::endl;
        tcp->close();
      };

      tcp->onDataEnd = [=] {
        std::cout << "data end" << std::endl;
        tcp->close();
      };

    };
  };

  loop->run();

  return 0;
}
