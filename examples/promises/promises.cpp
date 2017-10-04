#include <string>
#include <iostream>
#include <memory>

#include <satori/satori.hpp>

int main() {

  using namespace satori;

  // Create a Satori event loop
  auto satori = Satori();

  // Create two promises
  auto p = satori.promise<std::string>();
  auto w = satori.promise<std::string>();

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
  satori.run();

  return 0;
}
