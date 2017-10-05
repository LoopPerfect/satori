#include <gtest/gtest.h>
#include <satori/satori.hpp>
#include <string>
#include <vector>

TEST(satori, router) {

  auto const route = satori::createRoute("/products/:category/:id");

  auto const expected =
    neither::Maybe<std::vector<std::string>>({"bikes", "merckx"});

  auto const actual = route("/products/bikes/merckx");

  EXPECT_EQ(expected, actual);
}
