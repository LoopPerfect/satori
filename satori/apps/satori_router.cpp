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
      std::cout<< "error"<<std::endl;
  }

  router.compile();

  router.tree.dump(0);

  if(auto match = router.match("/products/bikes/merckx")) {
    match();
  }



  return 0;
}
