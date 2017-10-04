#ifndef SATORI_PROCESS_H
#define SATORI_PROCESS_H

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <uv.h>

#include <satori/handle.hpp>

namespace satori {

namespace detail {

// Utility method for deleting all memory pointed to
// by a uv_process_options_t object.
static void delete_uv_process_options_t(uv_process_options_t* options) {
  // args
  if (options->args) {
    int i = 0;
    while (options->args[i] != nullptr) {
      delete[] options->args[i];
      i += 1;
    }
    delete options->args;
    options->args = nullptr;
  }
}

template <class T = uv_process_t>
struct Process : Handle<T> {

  Process(uv_loop_t* loop) : Handle<T>(loop) {}

  ~Process() { delete_uv_process_options_t(&options); }

  int spawn(std::string const& file, std::vector<std::string> const& arguments,
            unsigned int flags = 0) {
    // TODO: Not all of the options properties have been ported.
    delete_uv_process_options_t(&options);
    options = {};
    options.exit_cb = [](uv_process_t* h, int64_t exit_status,
                         int term_signal) {
      auto* handle = (Process*)h;
      handle->onExit(exit_status, term_signal);
      handle->close();
    };
    options.file = file.c_str();
    options.flags = flags;
    options.env = nullptr;
    options.cwd = nullptr;
    options.args = new char*[arguments.size() + 1];
    for (int i = 0; i < arguments.size(); ++i) {
      options.args[i] = new char[std::strlen(arguments[i].c_str())];
      strcpy(options.args[i], arguments[i].c_str());
    }
    options.args[arguments.size()] = nullptr;
    return uv_spawn(this->loop, (uv_process_t*)this, &options);
  }

  uv_process_options_t options;

  std::function<void(int64_t, int)> onExit = [](int64_t, int) {};
};

} // namespace detail

using Process = detail::Process<>;
} // namespace satori

#endif
