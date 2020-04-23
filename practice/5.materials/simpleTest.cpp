#include "gtest/gtest.h"

TEST(MyTest, OnePlusOneIsTwo) {
  EXPECT_EQ(1 + 1, 2);
}

TEST(MyTest, OnePlusOneIsNotThree) {
  EXPECT_NE(1 + 1, 3);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
