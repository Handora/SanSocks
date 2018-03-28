/**
 * cipher_test.cpp
 */

#include "gtest/gtest.h"
#include "cipher/cipher.h"
#include <string>

namespace cipher {

  TEST(CipherTest, SimpleTest) {
    auto cipher_manager = Cipher::Instance();
    EXPECT_EQ(cipher_manager->decode(cipher_manager->encode("Hello")), std::string("Hello"));
  }

  // TEST(CipherTest, Base64Test) {
  //   int out_len;
  //   Base64 baser = Base64();
  //   std::string s2 = baser.base64_encode(s1, strlen(s1), &out_len);
  // }
}
