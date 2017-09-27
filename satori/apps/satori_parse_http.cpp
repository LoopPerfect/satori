#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <http_parser.h>
#include <satori/satori.hpp>

int main() {

  auto parser = std::make_shared<satori::HttpParser>(HTTP_REQUEST);

  parser->onUrl = [](http_parser const *parser, char const *at, size_t length) {
    std::cout << "URL: " << std::string(at, length) << std::endl;
    return 0;
  };

  parser->onHeaderField = [](http_parser const *parser, char const *at,
                             size_t length) {
    std::cout << std::string(at, length) << ": " << std::endl;
    return 0;
  };

  parser->onHeaderValue = [](http_parser const *parser, char const *at,
                             size_t length) {
    std::cout << std::string(at, length) << std::endl;
    return 0;
  };

  parser->onHeadersComplete = [](http_parser const *parser) {
    std::cout << "---" << std::endl;
    return 0;
  };

  // [](http_parser const*) { return 0; },
  // [](http_parser const*, const char*, size_t) { return 0; },
  // [](http_parser const*, const char*, size_t) { return 0; },
  // [](http_parser const*, const char*, size_t) { return 0; },
  // [](http_parser const*, const char*, size_t) { return 0; },
  // [](http_parser const*) { return 0; },
  // [](http_parser const*, const char*, size_t) { return 0; },
  // [](http_parser const*) { return 0; },
  // [](http_parser const*) { return 0; },
  // [](http_parser const*) { return 0; });

  auto const request =
    std::string("GET /test HTTP/1.1\r\n"
                "User-Agent: curl/7.18.0 (i486-pc-linux-gnu) libcurl/7.18.0 "
                "OpenSSL/0.9.8g zlib/1.2.3.3 libidn/1.1\r\n"
                "Host: 0.0.0.0=5000\r\n"
                "Accept: */*\r\n"
                "\r\n");

  auto result = parser->execute(request);

  std::cout << "Parsed " << result << " bytes. " << std::endl;

  return 0;
}
