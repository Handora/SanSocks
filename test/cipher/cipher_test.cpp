/**
 * cipher_test.cpp
 */

#include "gtest/gtest.h"
#include "cipher.h"
#include <string>

namespace cipher {

  TEST(CipherTest, SimpleTest) {
    auto cipher_manager = Cipher::Instance();
    EXPECT_EQ(cipher_manager->decode(cipher_manager->encode("Hello")), std::string("Hello"));
  }
}
