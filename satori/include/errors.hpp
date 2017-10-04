#ifndef SATORI_ERRORS_HPP
#define SATORI_ERRORS_HPP

#include <string>

namespace satori {

  std::string errorName(int const uvErrorCode) {
    return std::string(uv_err_name(uvErrorCode));
  }

  std::string errorMessage(int const uvErrorCode) {
    return std::string(uv_strerror(uvErrorCode));
  }

}

#endif
