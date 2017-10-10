#include <netinet/in.h>
#include <satori/dns.hpp>
#include <string>
#include <uv.h>

std::string satori::ipv4Name(sockaddr const &in) {
  char addr[17] = {'\0'};
  uv_ip4_name((sockaddr_in *)(&in), addr, 16);
  return std::string(addr);
}

std::string satori::ipv6Name(sockaddr const &in) {
  char addr[40] = {'\0'};
  uv_ip6_name((sockaddr_in6 *)(&in), addr, 39);
  return std::string(addr);
}
