#include <iostream>
#include <memory>
#include <string>
#include <iomanip>

#include <satori/satori.hpp>

int main(int argc, const char** argv) {

  using namespace satori;

  // Grab the executable path
  auto const path = !argv[1] ? "." : std::string(argv[1]);

  // Create a Satori event loop
  auto loop = std::make_shared<Loop>();

  // Create a new scan handle
  auto* fs = loop->newFSScanDir(path, 0);

  // Callback for each entry in the directory
  fs->onScandirNext = [](DirectoryEntry entry) {
    // Print the directory entry
    std::cout << std::setw(12)
      << directoryEntryTypeToString(entry.type)
      << " "
      << entry.name << std::endl;
    // Tell Satori to keep searching the directory
    return true;
  };

  // Callback for the start of the directory search
  fs->onScandir = [](int result) {
    // A negative value indicates an error
    if (result < 0) {
      // Print the error and crash
      std::cerr << errorName(result) + " " + errorMessage(result) << std::endl;
      exit(1);
    }
  };

  // Start the event loop
  loop->run();

  return 0;
}
