/**
 * cipher_test.cpp
 */

#include "gtest/gtest.h"
#include "cipher/cipher.h"
#include "cipher/base64.h"
#include "utility/util.h"

#include <string>

namespace sansocks {

  TEST(CipherTests, SimpleTest) {
    auto cipher_manager = Cipher::Instance();
    EXPECT_EQ(cipher_manager->Decode(cipher_manager->Encode("Hello")), std::string("Hello"));
  }

  TEST(CipherTests, CipherReadTest) {
    std::array<BYTE, TABLE_SIZE> encryption_test;

    for (int i = 0; i < TABLE_SIZE; i++) 
      encryption_test[i] = i;
    
    std::random_shuffle(encryption_test.begin(), encryption_test.end());

    Base64 base64er;
    std::string encoded_passwd = base64er.base64_encode(std::string(encoding_table.begin(), encoding_table.end()));
    auto cipher_manager = Cipher::Instance(encoded_passwd);

    std::string test_str = "Hello, world";
    std::string example_encoded_str = cipher_manager->Encode(test_str); 
    for (auto &s: test_str) {
      s = encryption_test[s];
    }
    
    EXPECT_EQ(test_str, example_encoded_str);
  }
}
