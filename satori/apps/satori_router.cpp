#include <functional>
#include <vector>
#include <string>
#include <iostream>

#include <satori/satori.hpp>

int main() {

  using namespace std;

  satori::Router<std::function<void(std::string, std::vector<std::string>)>> router;

  auto maybeError = router.addRoute(
    "/products/{category}/{id}",
    [](auto const& path, auto const& params) {
      std::cout << path << std::endl;
      for (auto const& e : params) {
        std::cout << e << std::endl;
      }
    });

  if (maybeError) {
    std::cerr << "Error: " << maybeError.get("") << std::endl;
    exit(1);
  }

  router.addRoute("/bikes/{id}", [](auto const& path, auto const& params) {
    std::cout << path << std::endl;
    for (auto const& e : params) {
      std::cout << e << std::endl;
    }
  });

  if (auto e = router.compile()) {
    std::cerr << "Error: " << maybeError.get("") << std::endl;
    exit(1);
  }

  // router.tree.dump(0);

  if (auto match = router.match("/products/bikes/merckx")) {
    auto const action = match.map([](auto x) -> std::function<void()> {
      return [=]() {
        x.data(x.path, x.params);
      };
    }).get([]() {});
    action();
  }

  if (auto match = router.match("/bikes/merckx")) {
    auto const action = match.map([](auto x) -> std::function<void()> {
      return [=]() {
        x.data(x.path, x.params);
      };
    }).get([]() {});
    action();
  }

  return 0;
}
