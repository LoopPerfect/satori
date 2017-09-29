#include <iostream>
// #include <r3.hpp>
#include <satori/satori.hpp>

int main() {

  using namespace std;

  satori::Router<> router;

  auto maybeError = router.addRoute("/products/{category}/{id}", [](auto const& req) {
    std::cout << req.path << std::endl;
    for(auto e: req.params) {
      std::cout << e << std::endl;
    }
  });

  if(maybeError) {
    std::cout << "error message: " << maybeError.get("") << std::endl;
  }

  router.addRoute("/bikes/{id}", [](auto const& req) {
    std::cout << req.path << std::endl;
    for(auto e: req.params) {
      std::cout << e << std::endl;
    }
  });

  if (auto e = router.compile()) {
    std::cout << "error"<< e << std::endl;
  }

  router.tree.dump(0);

  if (auto match = router.match("/products/bikes/merckx")) {
    match();
  }

  if(auto match = router.match("/bikes/merckx")) {
    match();
  }


  return 0;
}
