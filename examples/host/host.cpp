#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <satori/satori.hpp>

int main(int argc, const char** argv) {

  if (!argv[1]) {
    std::cout << "Usage: host <url>" << std::endl;
    return 1;
  }

  auto url = std::string(argv[1]);

  auto satori = satori::Satori();

  auto const onResolved = [=](int const status, std::vector<satori::uv_addrinfo> const& addrinfos) {
    if (status < 0) {
      std::cerr << "Non-zero status (" << status << ")" << std::endl;
      exit(1);
      return;
    }

    for (auto const& addrinfo : addrinfos) {
      if (addrinfo.ai_family == AF_INET) {
        std::cout << url << " has address " << satori::ipv4Name(*addrinfo.ai_addr) << std::endl;
      } else if (addrinfo.ai_family == AF_INET6) {
        std::cout << url << " has IPv6 address " << satori::ipv6Name(*addrinfo.ai_addr) << std::endl;
      } else {
        std::cerr << "Unrecognized address family (" << addrinfo.ai_family << ")" << std::endl;
      }
    }
  };

  satori.resolve(url, 80, onResolved);

  satori.run();

  return 0;
}
