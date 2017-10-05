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

    auto* fsRead = loop->newFSRead(file);

    fsRead->read((uv_loop_t*)loop.get(), file);

    fsRead->onRead = [=](int result, uv_buf_t buffer) {

      if (result < 0) {
        std::cerr << errorName(result) << " " << errorMessage(result) << std::endl;
        exit(1);
      }

      if (result < buffer.len) {
        std::cout << std::string(buffer.base, result) << std::flush;
      } else {
        std::cout << buffer.base;
        fsRead->read((uv_loop_t*)loop.get(), file, result);
      }
    };
  };

  loop->run();

  return 0;
}
