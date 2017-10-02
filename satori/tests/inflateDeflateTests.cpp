#include <string>
#include <functional>
#include <vector>
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

TEST(satori, chunkeddeflate) {

  auto const repeat = [](std::string const& x, unsigned const n) {
    std::vector<std::string> y;
    for (int i = 0; i < n; ++i) {
      y.push_back(x);
    }
    return y;
  };

  auto const join = [](std::vector<std::string> const& xs) {
    std::string y;
    for (auto const& x : xs) {
      y += x;
    }
    return y;
  };

  auto const xs = repeat("The slow black dog bows before the regal fox. ", 2048 * 2);

  auto deflator = satori::Deflator();

  for (auto const& x : xs) {
    deflator.feed(x);
  }

  deflator.finish();

  EXPECT_EQ(join(xs), satori::inflate(deflator.result()));
}
