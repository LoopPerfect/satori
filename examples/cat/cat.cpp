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

  auto* fsOpen = loop->newFSOpen(filePath, O_RDONLY, S_IRUSR);

  fsOpen->onOpen = [=](ssize_t file) {
    std::cout << "open" << std::endl;

    auto* fsRead = loop->newFSRead(file);

    fsRead->onRead = [=](int result, uv_buf_t buffer) {
      if (result < 0) {
        std::cerr << errorName(result) << " " << errorMessage(result) << std::endl;
        exit(1);
      } else if (result > 0) {
        std::cout << buffer.base;
      } else if (result <= buffer.len) {
        std::cout << buffer.base << std::endl;
      }
    };
  };

  loop->run();

  return 0;
}
