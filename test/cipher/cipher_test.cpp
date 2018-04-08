/**
 * cipher_test.cpp
 */

#include "gtest/gtest.h"
#include "cipher/cipher.h"
#include <string>

namespace sansocks {

  TEST(CipherTest, SimpleTest) {
    auto cipher_manager = Cipher::Instance();
    EXPECT_EQ(cipher_manager->Decode(cipher_manager->Encode("Hello")), std::string("Hello"));
  }
}
