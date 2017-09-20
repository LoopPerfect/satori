#include <iostream>
#include <string>
#include <memory>

#include <uv.h>
#include <satori/satori.hpp>


std::string error_to_string(int error) {
  return std::string(uv_err_name(error)) +
    " " +
    std::string(uv_strerror(error));
}

int main(int argc, const char ** argv) {

  using namespace Satori;

  if (!argv[1]) {
    std::cout << "Usage: host (without protocoll) " << std::endl;
    return 1;
  }

  auto loop = std::make_shared<Loop>();

  auto addrInfo = loop->newGetAddrInfo();
  addrInfo->resolve(argv[1], "80");
  addrInfo->onResolved = [=](auto s, auto addr) {
    addrInfo->close();

    auto tcp = loop->newTcp();
    auto connect = loop->newConnect();
    connect->onConnect = [=](int status) {
      std::cout << "connected "<< status << std::endl;

      auto writer = loop->newWrite();

      writer->write(tcp, "GET / HTTP/1.1\nHost: google.com:80\r\n\r\n");
      writer->onWriteEnd = [](auto...){
        std::cout << "write end" << std::endl;
      };
      tcp->read();
      tcp->onData = [=](char const* base, unsigned len) {
        std::cout << base << std::endl;
        tcp->close();
      };

      tcp->onDataEnd = [=](){
        std::cout << "data end" << std::endl;
        tcp->close();
      };

    };

    connect->connect(tcp, addr);
  };


  loop->run();

  return 0;
}
