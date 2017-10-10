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

  // Create a Satori event loop
  auto satori = Satori();

  // Create a callback to be called once the unlink is complete
  auto const onUnlink = [=](int const error) {
    if (error < 0) {
      std::cerr << errorName(error) << " " << errorMessage(error) << std::endl;
      exit(1);
    }
  };

  // Unlink the file
  satori.unlink(filePath, onUnlink);

  // Start the loop
  satori.run();

  return 0;
}
