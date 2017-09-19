#include <iostream>
#include <memory>

#include <uv.h>
#include <satori/satori.hpp>


int main(int argc, const char ** argv) {

  using namespace Satori;

  if (!argv[1]) {
    std::cout << "Usage: file_path " << std::endl;
    return 1;
  }

  auto loop = std::make_shared<Loop>();

  auto* fs = loop->takeFS();

  fs->onOpen = [](int status) {
    std::cout << "Callback here" << std::endl;
  };

  fs->open(
    argv[1],
    O_RDONLY,
    S_IRUSR
  );

  loop->run();

  return 0;
}
