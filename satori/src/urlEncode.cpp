#include <cctype>
#include <iomanip>
#include <satori/urlEncode.hpp>
#include <sstream>
#include <string>

namespace satori {

std::string urlEncode(std::string const& value) {

  std::ostringstream escaped;

  escaped.fill('0');
  escaped << std::hex;

  for (auto const& c : value) {

    // Keep alphanumeric and other accepted characters intact
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
      continue;
    }

    // Any other characters are percent-encoded
    escaped << std::uppercase;
    escaped << '%' << std::setw(2) << int((unsigned char)c);
    escaped << std::nouppercase;
  }

  return escaped.str();
}

} // namespace satori
