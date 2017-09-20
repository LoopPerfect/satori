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

  auto const path = !argv[1] ?
    "." :
    std::string(argv[1]);

  auto loop = std::make_shared<Loop>();

  auto* fs = loop->newFS();

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

  fs->scandir(path, 0);

  loop->run();

  return 0;
}
