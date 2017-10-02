#include <string>
#include <functional>
#include <gtest/gtest.h>
#include <satori/satori.hpp>

TEST(satori, inflatedeflate) {

  auto const inflateDeflate = [](std::string const& x) {
    EXPECT_EQ(x, satori::inflate(satori::deflate(x)));
  };

  auto const repeat = [](std::string const& x, unsigned const n) {
    std::string y;
    for (int i = 0; i < n; ++i) {
      y += x;
    }
    return y;
  };

  inflateDeflate("");
  inflateDeflate("abdefghijklmnopqrstuvwxyz0123456789");
  inflateDeflate("The slow black dog bows before the regal fox. ");
  inflateDeflate("hello world");
  inflateDeflate("a   b");
  inflateDeflate("ab\"cd");
  inflateDeflate("hello\nworld");
  inflateDeflate(repeat("The slow black dog bows before the regal fox. ", 2048));
}
