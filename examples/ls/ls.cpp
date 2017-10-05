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
  auto satori = Satori();

  // Callback for the start of the directory search
  auto const scanCallback = [](int const result) {
    // A negative value indicates an error
    if (result < 0) {
      // Print the error and crash
      std::cerr << errorName(result) + " " + errorMessage(result) << std::endl;
      exit(1);
    }
  };

  // Callback for each entry in the directory
  auto const entryCallback = [](DirectoryEntry const& entry) {
    // Print the directory entry
    std::cout << std::setw(12)
      << directoryEntryTypeToString(entry.type)
      << " "
      << entry.name
      << std::endl;
    // Tell Satori to keep searching the directory
    return true;
  };

  // Request a new directory scan
  satori.scanDirectory(path, scanCallback, entryCallback);

  // Start the event loop
  satori.run();

  return 0;
}
