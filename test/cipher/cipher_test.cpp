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
    std::vector<BYTE> encryption_test;

    encryption_test.resize(TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++) 
      encryption_test[i] = i;
    
    std::random_shuffle(encryption_test.begin(), encryption_test.end());

    Base64 base64er;
    std::vector<BYTE> encoded_passwd = base64er.base64_encode(encryption_test);
    auto cipher_manager = Cipher::Instance(std::string(encoded_passwd.begin(), encoded_passwd.end()));

    std::string test_str = "Hello, worldssdasdsadas";
    std::string example_encoded_str = cipher_manager->Encode(test_str); 
    for (auto &s: test_str) {
      s = encryption_test[(BYTE)s];
      std::cout << (int)s << std::endl;
    }
    
    EXPECT_EQ(true, test_str == example_encoded_str);
  }
}
