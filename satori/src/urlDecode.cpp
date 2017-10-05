#include <satori/urlDecode.hpp>
#include <string>

namespace satori {

// From https://stackoverflow.com/a/29962178/1256041
std::string urlDecode(std::string const& str) {
  std::string ret;
  char ch;
  int i;
  int ii;
  int len = str.length();
  for (i = 0; i < len; i++) {
    if (str[i] != '%') {
      if (str[i] == '+') {
        ret += ' ';
      } else {
        ret += str[i];
      }
    } else {
      sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
      ch = static_cast<char>(ii);
      ret += ch;
      i = i + 2;
    }
  }
  return ret;
}

} // namespace satori
