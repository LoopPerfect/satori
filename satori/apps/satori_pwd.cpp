#include <iostream>
#include <memory>
#include <string>

#include <satori/satori.hpp>
#include <uv.h>

std::string error_to_string(int error) {
  return std::string(uv_err_name(error)) + " " +
         std::string(uv_strerror(error));
}

int main(int argc, const char **argv) {

  using namespace satori;

  auto loop = std::make_shared<Loop>();

  auto *fs = loop->newFSRealPath(".");

  fs->onError = [](int error) {
    std::cerr << error_to_string(error) << std::endl;
    exit(1);
  };

  fs->onRealpath = [](std::string path) { std::cout << path << std::endl; };

  loop->run();

  return 0;
}
