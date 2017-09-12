#include <map>
#include <string>
#include <satori/http-parser.hpp>
#include <gtest/gtest.h>

TEST(satori, httpparser) {

  auto expected = std::map<std::string, std::string>({
    { "method", "GET" },
    { "path", "/" }
  });

  auto actual = Satori::parseReq("GET / HTTP/1.1\nHost: localhost:8082");

  EXPECT_EQ(expected, actual);
}
