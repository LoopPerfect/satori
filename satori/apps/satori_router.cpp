#include <iostream>
// #include <r3.hpp>
#include <satori/satori.hpp>

int main() {

  using namespace std;

  satori::Router router;

  auto maybeError = router.addRoute("/products/:category/:id", []() {
    std::cout << "blub" << std::endl;
  });

  router.compile();

  router.tree.dump(0);

  router.match("/products/bikes/merckx");

  return 0;
}
