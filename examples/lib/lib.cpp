#include <iostream>
#include <memory>
#include <functional>
#include <string>

#include <satori/satori.hpp>

int main(int argc, const char** argv) {

  auto dynamicLibrary = satori::DynamicLibrary();

  auto openResult = dynamicLibrary.open("libmath.dylib");

  if (openResult) {
    std::cerr << openResult << std::endl;
    return 1;
  }

  auto findResult = dynamicLibrary.find("add");

  if (findResult.left()) {
    std::cerr << findResult.left() << std::endl;
    return 1;
  }

  using AddT = int(int, int);
  
  findResult.right().map([](auto addSymbol) -> std::function<void()> {
    return [=]() {
      auto* add = (AddT*)addSymbol.get();
      std::cout << "add(2, 3) = " << add(2, 3) << std::endl;
    };
  }).get([]() {})();

  // RAII is used to close the library

  return 0;
}
