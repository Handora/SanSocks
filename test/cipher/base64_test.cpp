/* 
 * base64_test.cpp
 */

#include <memory>

#include "gtest/gtest.h"
#include "cipher/base64.h"

namespace sansocks {
  
  TEST(Base64Tests, SimpleTest) {
    auto baser = new Base64();
    std::string str = "Hello, world";
    std::string res = baser->base64_encode(str);
    std::cout << res << std::endl; 
    EXPECT_EQ(str, baser->base64_decode(res));
  }
} // namespace sansocks
