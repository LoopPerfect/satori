#include <gtest/gtest.h>
#include <satori/satori.hpp>

TEST(satori, managed_ptr) {

  using namespace satori;

  auto a = managed_ptr<std::string>(new std::string("abc"));
  auto b = managed_ptr<std::string>(new std::string("abc"));
  auto c = managed_ptr<std::string>(a.get());
  auto d = managed_ptr<std::string>(nullptr);
  auto e = a;

  EXPECT_EQ(a, c);
  EXPECT_NE(a, b);
  EXPECT_EQ(a.get(), c.get());
  EXPECT_EQ(a, e);

  EXPECT_EQ(3, a->size());
  EXPECT_EQ(3, b->size());
  EXPECT_EQ(*a, *b);
  EXPECT_EQ(*a, *c);

  EXPECT_TRUE(a);
  EXPECT_TRUE(b);
  EXPECT_TRUE(c);
  EXPECT_FALSE(d);

  delete a.get();
  delete b.get();
  // c == a so c.get() is already deleted
  // d.get() == nullptr
  // e == a so e.get() is already deleted
}
