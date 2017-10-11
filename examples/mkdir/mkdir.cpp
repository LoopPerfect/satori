#include <iostream>
#include <memory>
#include <functional>
#include <string>

#include <satori/satori.hpp>

int main(int argc, const char** argv) {

  using namespace satori;

  // Take a path from the command-line arguments
  if (!argv[1]) {
    std::cerr << "Usage: directory_name " << std::endl;
    return 1;
  }

  auto directoryName = std::string(argv[1]);

  // Create a Satori event-loop
  auto satori = Satori();

  // Create a callback to be invoked once mkdir completes
  auto const onMkdir = [=](int const status) {
    // Was there an error?
    if (status < 0) {
      std::cerr << errorName(status) << " " << errorMessage(status) << std::endl;
      exit(1);
      return;
    }
    std::cout << "Created a directory at " << directoryName << std::endl;
  };

  // Queue up a call to mkdir
  satori.mkdir(directoryName, 0777, onMkdir);

  // Start the loop!
  satori.run();

  return 0;
}
