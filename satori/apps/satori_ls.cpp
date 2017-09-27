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

  auto const path = !argv[1] ? "." : std::string(argv[1]);

  auto loop = std::make_shared<Loop>();

  auto* fs = loop->newFSScanDir(path, 0);

  fs->onScandirNext = [](uv_dirent_t ent) {
    std::cout << ent.type << " " << ent.name << std::endl;
    return true;
  };

  fs->onScandir = [](int result) {
    if (result < 0) {
      std::cerr << error_to_string(result) << std::endl;
      exit(1);
    }
  };

  loop->run();

  return 0;
}
