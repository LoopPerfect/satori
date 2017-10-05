#include <gtest/gtest.h>
#include <satori/satori.hpp>
#include <string>

TEST(satori, urlencode) {

  EXPECT_EQ("abdef", satori::urlEncode("abdef"));
  EXPECT_EQ("hello%20world", satori::urlEncode("hello world"));
  EXPECT_EQ("a%20%20%20b", satori::urlEncode("a   b"));
  EXPECT_EQ("ab%22cd", satori::urlEncode("ab\"cd"));
  EXPECT_EQ("hello%0Aworld", satori::urlEncode("hello\nworld"));
  EXPECT_EQ("http%3A%2F%2Ffoo%20bar%2F", satori::urlEncode("http://foo bar/"));
}

TEST(satori, urlencodedecode) {

  auto const encodeDecode = [](std::string const& x) {
    EXPECT_EQ(x, satori::urlDecode(satori::urlEncode(x)));
  };

  encodeDecode("");
  encodeDecode("abdef");
  encodeDecode("hello world");
  encodeDecode("a   b");
  encodeDecode("ab\"cd");
  encodeDecode("hello\nworld");
}
