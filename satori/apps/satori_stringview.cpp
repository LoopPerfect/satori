#include <cstring>
#include <map>
#include <string>

template <class T>
struct Range {
  T const* b;
  T const* e;

  Range() : b{0}, e{0} {}

  Range(T const* b, T const* e) : b{b}, e{e} {}

  template <size_t n>
  Range(T cstr[n]) : b(cstr), e(cstr + n) {}

  Range(std::string const& str) : b(str.c_str()), e(str.c_str() + str.size()) {}

  T const* begin() const { return b; }
  T const* end() const { return e; }
  size_t size() const { return e - b; }
};

template <class T>
int compare(Range<T> const lhs, Range<T> const rhs) {
  auto const c =
    memcmp(lhs.begin(), rhs.begin(), std::min(lhs.size(), rhs.size()));
  if (c)
    return c;
  return rhs.size() - lhs.size();
}

template <class T>
bool operator<(Range<T> const lhs, Range<T> const rhs) {
  return compare(lhs, rhs) < 0;
}

int main() {

  char str[] = "a = b; c = d; e = f";

  std::map<Range<char>, Range<char>> strings;

  strings[{&str[0], &str[1]}] = {&str[4], &str[5]};

  return 0;
}
