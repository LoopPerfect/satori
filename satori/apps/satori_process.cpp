#include <iostream>
#include <string>
#include <memory>
#include <cstdio>

#include <uv.h>
#include <satori/satori.hpp>

std::string error_to_string(int error) {
  return std::string(uv_err_name(error)) +
    " " +
    std::string(uv_strerror(error));
}

int main(int argc, const char ** argv) {

  using namespace ;

  auto loop = std::make_shared<Loop>();

  auto* process = loop->newProcess();

  process->onExit = [](int64_t exit_status, int term_signal) {
    std::cout << "I'm back! " << std::endl;
    std::cout << "exit_status " << exit_status
              << " term_signal " << term_signal
              << std::endl;
  };

  int r = process->spawn("sleep", { "sleep", "1" });

  if (r < 0) {
    std::cout << error_to_string(r) << std::endl;
    return 1;
  }

  r = loop->run();

  if (r < 0) {
    std::cout << error_to_string(r) << std::endl;
    return 1;
  }
  
  return 0;
}
