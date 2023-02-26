// #include <gtest/gtest.h>
// how to get this to work with cmake?
// ?? First, you need to include the header file for the class you want to test


// include the header file for the class you want to test
#include "../src/reader.h"

// Demonstrate some basic assertions.
// TEST(HelloTest, BasicAssertions) {
//   // Expect two strings not to be equal.
//   EXPECT_STRNE("hello", "world");
//   // Expect equality.
//   EXPECT_EQ(7 * 6, 42);
// }

void t1()
{
  std::string source = "(define (factorial n) (if (= n 0) 1 (* n (factorial (- n 1)))))";
  Tokenizer tokenizer(source);
  while (true)
  {
    auto [type, token] = tokenizer.next_token();
    if (type == TOKEN_TYPE::EOF_TOKEN)
    {
      break;
    }
    std::cout << token << std::endl;
  }
}

int main(int argc, char** argv) {
  // ::testing::InitGoogleTest(&argc, argv);
  // return RUN_ALL_TESTS();
  t1();


}