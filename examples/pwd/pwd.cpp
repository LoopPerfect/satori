#include <iostream>
#include <memory>
#include <string>

#include <satori/satori.hpp>

int main(int argc, const char** argv) {

  using namespace satori;

  auto satori = Satori();

  auto const realPathCallback = [](std::string const& path) {
    std::cout << path << std::endl;
  };

  auto const errorCallback = [](int const error) {
    std::cerr << errorName(error) << " " << errorMessage(error) << std::endl;
    exit(1);
  };

  satori.realPath(".", realPathCallback, errorCallback);

  satori.run();

  return 0;
}
