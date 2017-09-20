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
    std::cout << "Usage: file_path " << std::endl;
    return 1;
  }

  auto loop = std::make_shared<Loop>();

  auto* fs = loop->newFS();

  double atime = 1000;
  double mtime = 2000;

  fs->onUtime = [&](int result) {
    if (result < 0) {
      std::cerr << error_to_string(result) << std::endl;
      exit(1);
    }
    std::cout << "atime " << atime << " mtime " << mtime << std::endl;
  };

  fs->utime(argv[1], atime, mtime);

  loop->run();

  return 0;
}
