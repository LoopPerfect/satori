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

  auto loop = std::make_shared<Loop>();

  auto* fs = loop->newFS();

  fs->onError = [](int error) {
    std::cerr << error_to_string(error) << std::endl;
    exit(1);
  };

  fs->onRealpath = [](std::string path) {
    std::cout << path << std::endl;
  };

  fs->realpath(".");

  loop->run();

  return 0;
}
