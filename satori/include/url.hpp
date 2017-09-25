#ifndef SATORI_URL_HPP
#define SATORI_URL_HPP

#include <vector>
#include <string>

namespace satori {

  struct Url {
    int port;
    std::vector<std::string> parts;
  };

  bool operator == (Url const& lhs, Url const& rhs) {
    return lhs.port == rhs.port && lhs.parts == rhs.parts;
  }

  bool operator != (Url const& lhs, Url const& rhs) {
    return !(lhs == rhs);
  }

}

#endif
