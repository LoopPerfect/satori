#include <map>
#include <string>
#include <neither/either.hpp>
#include <satori/satori.hpp>
#include <gtest/gtest.h>

TEST(satori, urlparser) {

  satori::Url url = {
    80,
    {
      "https",
      "www.cultofthepartyparrot.com",
      "80",
      "/"
    }
  };

  auto expected = neither::Either<int, satori::Url>::rightOf(url);

  auto actual = satori::parseUrl("https://www.cultofthepartyparrot.com:80/");

  EXPECT_EQ(expected, actual);

  // auto a = neither::Either<int, satori::Url>::rightOf(url);
  // auto b = neither::Either<int, satori::Url>::rightOf(url);
  //
  // EXPECT_EQ(a, b);

}
