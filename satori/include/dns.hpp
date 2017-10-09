#ifndef SATORI_DNS_HPP
#define SATORI_DNS_HPP

#include <string>
#include <uv.h>

namespace satori {

  std::string ipv4Name(sockaddr const& in);

  std::string ipv6Name(sockaddr const& in);
}

#endif
