#include <iostream>
#include <memory>
#include <functional>
#include <string>

#include <satori/satori.hpp>
#include <uv.h>

int main(int argc, const char** argv) {

  using namespace satori;

  // Take a path from the command-line arguments
  if (!argv[1]) {
    std::cerr << "Usage: file_path " << std::endl;
    return 1;
  }

  auto filePath = std::string(argv[1]);

  // Create a Satori event loop
  auto satori = Satori();

  auto const onOpen = [=](ssize_t const file) mutable {

    auto const onRead = [=](int error, StringView const buffer) mutable {
      if (error < 0) {
        std::cerr << errorName(error) << " " << errorMessage(error) << std::endl;
        exit(1);
        return;
      }
      std::cout << buffer;
    };

    auto const onReadEnd = [=]() mutable {
      std::cout << std::flush;
      satori.closeFile(file);
    };

    satori.readFile(file, onRead, onReadEnd);
  };

  satori.openFile(filePath, O_RDONLY, S_IRUSR, onOpen);

  // Start the loop
  satori.run();

  return 0;
}
