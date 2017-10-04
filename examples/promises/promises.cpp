#include <string>
#include <iostream>
#include <memory>

#include <satori/satori.hpp>

int main() {

  using namespace satori;

  // Create a Satori event loop
  auto loop = std::make_shared<Loop>();

  // Create two promises
  auto p = Promise<std::string>(std::weak_ptr<Loop>(loop));
  auto w = Promise<std::string>(std::weak_ptr<Loop>(loop));

  // q is a transformation of p
  auto q = p.map([=](std::string x) {
    return x + "! " + x;
  });

  // Set the value of p
  p.resolve("Hello");

  // r is a transformation of q
  auto r = q.flatMap([=](std::string x) {
    return w.map([=](std::string y) {
      return x + "... " + y;
    });
  });

  // Perform an action once r is resolved
  r.onResolve([=](std::string x) {
    std::cout << x << std::endl;
  });

  // Resolve w
  w.resolve("world. ");

  // Start the loop
  loop->run();

  return 0;
}
