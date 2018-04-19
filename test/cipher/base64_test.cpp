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
    std::vector<BYTE> vb(str.begin(), str.end());
    auto res = baser->base64_encode(vb); 
    EXPECT_EQ(vb, baser->base64_decode(res));
  }
} // namespace sansocks
