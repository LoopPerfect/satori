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

  fs->onOpen = [=](ssize_t file) {

    fs->onWrite = [=](int result) {
      if (result < 0) {
        std::cerr << error_to_string(result) << std::endl;
        exit(1);
      }
      std::cout << "Wrote " << result << " bytes. " << std::endl;
      fs->onClose = []() {
        std::cout << "Closed. " << std::endl;
      };
      fs->close(file);
    };

    auto const repeat = [](std::string const& x, std::size_t n) {
      std::string y = "";
      for (int i = 0; i < n; ++i) {
        y += x;
      }
      return y;
    };

    auto message = repeat("Hello, world. ", 100);

    fs->write(file, message.c_str(), message.length());
  };

  fs->open(
    argv[1],
    O_CREAT | O_RDWR,
    S_IRUSR);

  loop->run();

  return 0;
}
