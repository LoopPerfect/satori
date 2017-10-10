#include <iostream>
#include <functional>
#include <string>

#include <satori/satori.hpp>

int main(int argc, const char** argv) {

  using namespace satori;

  // Take a path from the command-line arguments
  if (!argv[1]) {
    std::cerr << "Usage: file_path " << std::endl;
    return 1;
  }

  auto filePath = std::string(argv[1]);

  // Create an event loop
  auto loop = std::make_shared<satori::Loop>();

  auto fsPoll = loop->newFSPoll();

  fsPoll->onPoll = [=](int const status, uv_stat_t const& previous, uv_stat_t const& current) {

    // Report any errors and crash
    if (status < 0) {
      std::cerr << errorName(status) << " " << errorMessage(status) << std::endl;
      exit(1);
      return;
    }

    // Print the new mtime
    std::cout << "current.st_mtim.tv_sec " << current.st_mtim.tv_sec << std::endl;

    // To stop watching:
    // fsPoll->stop();
  };

  // Poll the file every second
  fsPoll->start(filePath, 1000);

  loop->run();

  return 0;
}
