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

    fsRead->read();

    fsRead->onRead = [=](int error, StringView const buffer) {
      std::cout << buffer.toString() << std::flush;

      // if (!buffer) {
      //   fsRead->stop();
      // }
    };

  };

  loop->run();

  return 0;
}
