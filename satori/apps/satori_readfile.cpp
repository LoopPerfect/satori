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

  using namespace;

  if (!argv[1]) {
    std::cout << "Usage: file_path " << std::endl;
    return 1;
  }

  auto loop = std::make_shared<Loop>();

  FS *fs = loop->newFS();

  fs->onOpen = [=](ssize_t file) {

    fs->onRead = [=](int result, uv_buf_t buffer) {
      if (result < 0) {
        std::cerr << error_to_string(result) << std::endl;
        exit(1);
      } else if (result > 0) {
        std::cout << buffer.base;
      }

      if (result < buffer.len) {
        fs->onClose = []() {
          std::cout << "=================EOF=================" << std::endl;
        };
        fs->close(file);
      }
    };

    fs->read(file);
  };

  fs->open(argv[1], O_RDONLY, S_IRUSR);

  loop->run();

  return 0;
}
