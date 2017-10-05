#include <iostream>
#include <memory>
#include <string>

#include <satori/satori.hpp>
#include <uv.h>

int main(int argc, const char** argv) {

  using namespace satori;

  if (!argv[1]) {
    std::cout << "Usage: file_path " << std::endl;
    return 1;
  }

  auto filePath = std::string(argv[1]);

  auto loop = std::make_shared<Loop>();

  auto* open = loop->newFSOpen(filePath, O_RDONLY, S_IRUSR);

  fs->onOpen = [=](ssize_t file) {
    std::cout << "open" << std::endl;

    // fs->onRead = [=](int result, uv_buf_t buffer) {
    //   if (result < 0) {
    //     std::cerr << errorName(result) << " " << errorMessage(result) << std::endl;
    //     exit(1);
    //   } else if (result > 0) {
    //     std::cout << buffer.base;
    //   }
    //
    //   if (result < buffer.len) {
    //     fs->onClose = []() {
    //       std::cout << "=================EOF=================" << std::endl;
    //     };
    //     fs->close(file);
    //   }
    // };
    //
    // fs->read(file);
  };

  // fs->open(loop.get(), filePath, O_RDONLY, S_IRUSR);

  loop->run();

  return 0;
}
