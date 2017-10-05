#include <iostream>
#include <memory>
#include <string>

#include <satori/satori.hpp>

int main(int argc, const char** argv) {

  using namespace satori;

  auto satori = Satori();

  satori.realPath(
    ".",
    [](std::string const& path) {
      std::cout << path << std::endl;
    },
    [](int const error) {
      std::cerr << errorName(error) << " " << errorMessage(error) << std::endl;
      exit(1);
    });

  satori.run();

  return 0;
}
