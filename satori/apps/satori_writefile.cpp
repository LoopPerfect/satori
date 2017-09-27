#include <iostream>
#include <memory>
#include <string>

#include <satori/satori.hpp>
#include <uv.h>

std::string error_to_string(int error) {
  return std::string(uv_err_name(error)) + " " +
         std::string(uv_strerror(error));
}

auto const repeat = [](std::string const& x, std::size_t n) {
  std::string y = "";
  for (int i = 0; i < n; ++i) {
    y += x;
  }
  return y;
};

int main(int argc, const char** argv) {

  using namespace satori;

  if (!argv[1]) {
    std::cout << "Usage: file_path " << std::endl;
    return 1;
  }

  auto loop = std::make_shared<Loop>();

  loop->newFSOpen(argv[1], O_CREAT | O_RDWR, S_IRUSR)->onOpen =
    [=](ssize_t file) {
      auto msg = repeat("Hello, world. ", 100);
      loop->newFSWrite(file, msg)->onWrite = [=](int result) {
        if (result < 0) {
          std::cerr << error_to_string(result) << std::endl;
          exit(1);
        }
        std::cout << "Wrote " << result << " bytes. " << std::endl;

        loop->newFSClose(file)->onClose = [] {
          std::cout << "file closed" << std::endl;
        };

      };
    };

  loop->run();

  return 0;
}
