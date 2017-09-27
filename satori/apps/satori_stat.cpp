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

  if (!argv[1]) {
    std::cout << "Usage: file_path " << std::endl;
    return 1;
  }

  auto loop = std::make_shared<Loop>();

  auto *fs = loop->newFSStat(argv[1]);

  fs->onStat = [](int result, uv_stat_t statbuf) {
    if (result < 0) {
      std::cerr << error_to_string(result) << std::endl;
      exit(1);
    }
    std::cout << "st_size " << statbuf.st_size << std::endl;
    std::cout << "st_mtim " << statbuf.st_mtim.tv_sec << "s "
              << statbuf.st_mtim.tv_nsec << "ns" << std::endl;
  };

  loop->run();

  return 0;
}
