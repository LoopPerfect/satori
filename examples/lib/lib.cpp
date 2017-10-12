#include <iostream>
#include <memory>
#include <functional>
#include <string>

#include <satori/satori.hpp>

int main() {

  auto dynamicLibrary = satori::DynamicLibrary();

  // Open a library
  auto openResult = dynamicLibrary.open("libmath.dylib");

  if (openResult) {
    std::cerr << openResult << std::endl;
    return 1;
  }

  // Find and use the add function
  using AddT = int(int, int);

  auto findResult = dynamicLibrary.find<AddT>("add");

  if (findResult.left()) {
    std::cerr << findResult.left() << std::endl;
    return 1;
  }

  findResult.right().map([](auto addSymbol) -> std::function<void()> {
    return [=]() {
      auto* add = addSymbol.get();
      std::cout << "add(2, 3) = " << add(2, 3) << std::endl;
    };
  }).get([]() {})();

  // Smart pointers are used to close the library

  return 0;
}
