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

void on_exit(uv_process_t *req, int64_t exit_status, int term_signal) {
  // fprintf(stderr, "Process exited with status %" PRId64 ", signal %d\n",
  //   exit_status, term_signal);
  std::cout << "exit" << std::endl;
  uv_close((uv_handle_t*) req, NULL);
}

int main(int argc, const char ** argv) {

  using namespace Satori;

  auto loop = std::make_shared<Loop>();

  auto* process = loop->newProcess();

  uv_process_options_t options = {};

  char* args[1];
  args[0] = "pwd";

  options.exit_cb = on_exit;
  options.file = "pwd";
  options.flags = UV_PROCESS_DETACHED;
  options.args = args;
  options.stdio_count = 3;

  int r = process->spawn(&options);

  std::cout << r << std::endl;

  // fs->onScandirNext = [](uv_dirent_t ent) {
  //   std::cout << ent.type << " " << ent.name << std::endl;
  //   return true;
  // };

  // fs->onScandir = [](int result) {
  //   if (result < 0) {
  //     std::cerr << error_to_string(result) << std::endl;
  //     exit(1);
  //   }
  // };

  // fs->scandir(path, 0);

  loop->run();

  return 0;
}
