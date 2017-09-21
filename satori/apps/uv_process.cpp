#include <iostream>
#include <string>
#include <memory>
#include <cstring>

#include <uv.h>

char* a;
char* b;

char** args;

std::string error_to_string(int const& error) {
  return std::string(uv_err_name(error)) +
    " " +
    std::string(uv_strerror(error));
}

void on_exit(uv_process_t* req, int64_t exit_status, int term_signal) {
  std::cout << "I'm back! " << std::endl;
  std::cout << "exit_status " << exit_status
    << " term_signal " << term_signal << std::endl;

  uv_close((uv_handle_t*)req, nullptr);

  delete a;
  delete b;
  delete args;
}

int main(int argc, const char** argv) {

  auto* loop = new uv_loop_t();

  uv_loop_init(loop);

  auto* process = new uv_process_t();

  uv_process_options_t options = {};

  a = new char[100];
  b = new char[100];

  strcpy(a, "sleep");
  strcpy(b, "1");

  args = new char*[3];
  args[0] = a;
  args[1] = b;
  args[2] = nullptr;

  options.exit_cb = on_exit;
  options.file = "sleep";
  options.flags = 0;
  options.args = args;

  std::cout << "Going to sleep..." << std::endl;

  int const r = uv_spawn(loop, process, &options);

  if (r < 0) {
    std::cout << error_to_string(r) << std::endl;
    return 1;
  }

  uv_run(loop, UV_RUN_DEFAULT);

  delete process;
  delete loop;

  return 0;
}
