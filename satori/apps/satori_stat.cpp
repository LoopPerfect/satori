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

  using namespace ;

  if (!argv[1]) {
    std::cout << "Usage: file_path " << std::endl;
    return 1;
  }

  auto loop = std::make_shared<Loop>();

  auto* fs = loop->newFS();

  fs->onStat = [](int result, uv_stat_t statbuf) {
    if (result < 0) {
      std::cerr << error_to_string(result) << std::endl;
      exit(1);
    }
    std::cout << "st_size " << statbuf.st_size << std::endl;
    std::cout << "st_mtim " << statbuf.st_mtim.tv_sec << "s " <<
      statbuf.st_mtim.tv_nsec << "ns" << std::endl;
  };

  fs->stat(argv[1]);

  loop->run();

  return 0;
}
