#include <string>
#include <iostream>
#include <memory>

#include <satori/satori.hpp>

int main() {

  using namespace satori;

  // auto loop = std::make_shared<Loop>();

  Promise<std::string> promise{}; //createPromise<std::string>(loop.get());

  promise.onResolve([](std::string x) {
    std::cout << x << std::endl;
  });

  promise.resolve("Hello, world. ");

  // loop->run();

  return 0;
}
