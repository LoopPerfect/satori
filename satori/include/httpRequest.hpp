#ifndef SATORI_HTTP_REQUEST_HPP
#define SATORI_HTTP_REQUEST_HPP

#include <string>
#include <map>
#include <vector>

namespace satori {

struct HttpRequest {
  std::string url;
  std::string method;
  std::map<std::string, std::string> headers;
  std::vector<std::string> params;
};

}

#endif
