#include <string>
#include <iostream>
#include <memory>

#include <satori/satori.hpp>

int main() {

  using namespace satori;

  auto loop = std::make_shared<Loop>();

  auto p = Promise<std::string>(std::weak_ptr<Loop>(loop));
  auto w = Promise<std::string>(std::weak_ptr<Loop>(loop));

  auto q = p.map([=](std::string x) {
    return x + x;
  });

  p.resolve("Hello");

  auto r = q.flatMap([=](std::string x) {
    return w.map([=](std::string y) {
      return x + "... " + y;
    });
  });

  r.onResolve([=](std::string x) {
    std::cout << x << std::endl;
  });

  w.resolve("world. ");

  loop->run();

  return 0;
}
