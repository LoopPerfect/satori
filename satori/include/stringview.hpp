#ifndef SATORI_STRINGVIEW_HPP
#define SATORI_STRINGVIEW_HPP

#include <cstring>
#include <string>
#include <ostream>

namespace satori {

struct StringView {
  char const* b;
  char const* e;

  constexpr StringView() : b{0}, e{0} {}

  constexpr StringView(char const* b, char const* e) : b{b}, e{e} {}
  constexpr StringView(char const* b, size_t size) : b{b}, e{b+size} {}

  template<class T>
  constexpr StringView(T const& buf) 
    : b{buf.base}, e{buf.base+buf.len} 
  {}

  template <size_t n>
  constexpr StringView(char (&cstr)[n]) : b(cstr), e(cstr + n) {}

  StringView(std::string const& str) : b(str.c_str()), e(str.c_str() + str.size()) {}

  char const* begin() const { return b; }
  char const* end() const { return e; }
  size_t size() const { return e - b; }

  constexpr operator bool() const {
    return b < e;
  }

  std::string toString() const {
    return {b, e};
  }

  explicit operator std::string() const {
    return toString();
  }
};

int compare(StringView const lhs, StringView const rhs) {
  auto const c =
    memcmp(lhs.begin(), rhs.begin(), std::min(lhs.size(), rhs.size()));
  if (c)
    return c;
  return rhs.size() - lhs.size();
}

bool operator < (StringView const lhs, StringView const rhs) {
  return compare(lhs, rhs) < 0;
}

bool operator > (StringView const lhs, StringView const rhs) {
  return compare(lhs, rhs) > 0;
}

bool operator == (StringView const lhs, StringView const rhs) {
  return compare(lhs, rhs) == 0;
}

bool operator <= (StringView const lhs, StringView const rhs) {
  return compare(lhs, rhs) <= 0;
}

bool operator >= (StringView const lhs, StringView const rhs) {
  return compare(lhs, rhs) >= 0;
}

constexpr StringView operator ""_sv (char const* str , size_t const size) {
  return StringView(str, size);
}

std::ostream& operator<< (std::ostream& os, StringView const sv) {
  os.write(sv.begin(), sv.size());
  return os;
}

}

#endif
