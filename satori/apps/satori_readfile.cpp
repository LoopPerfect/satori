#include <iostream>

int main(int argc, const char ** argv) {
  if (!argv[1]) {
    std::cout << "Usage: file_path " << std::endl;
    return 1;
  }

  auto loop = std::make_shared<Loop>();

  auto* fs = loop->takeFS();

  return 0;
}
