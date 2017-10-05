#include <gtest/gtest.h>
#include <satori/stringview.hpp>
#include <string>

TEST(satori, stringview) {

  using namespace satori;
  
  std::string foo = "hello world";

  StringView sv(foo);

  EXPECT_EQ(true, (sv.toString() == foo));


  StringView a("00001");
  StringView b("00002");

  EXPECT_EQ(true, a < b);


  EXPECT_EQ(true, a < "00002"_sv );
}
