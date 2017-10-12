#ifndef SATORI_LIB_HPP
#define SATORI_LIB_HPP

#include <string>
#include <uv.h>

namespace satori {

  struct Lib : uv_lib_t {

    ~Lib() {
      close();
    }

    int open(std::string const& filename) {
      // int uv_dlopen(const char* filename, uv_lib_t* lib)
      return uv_dlopen(filename.c_str(), (uv_lib_t*)this);
    }

    void close() {
      // void uv_dlclose(uv_lib_t* lib)
      uv_dlclose((uv_lib_t*)this);
    }

    int dlsyn(std::string const& name, void** ptr) {
      // int uv_dlsym(uv_lib_t* lib, const char* name, void** ptr)
      return uv_dlsym((uv_lib_t*)this, name.c_str(), ptr);
    }

    std::string dlerror() {
      // UV_EXTERN const char* uv_dlerror(const uv_lib_t* lib);
      return std::string(uv_dlerror((uv_lib_t*)this));
    }

  };

}

#endif
