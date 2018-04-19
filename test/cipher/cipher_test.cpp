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

  TEST(CipherTests, RandomTest) {
    auto cipher_manager = Cipher::Instance();
    
    static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

    srand(time(nullptr));
    std::string s;
    for (int i = 0; i < 10000; ++i) {
      s.push_back(alphanum[rand() % (sizeof(alphanum) - 1)]);
    }

    EXPECT_EQ(s, cipher_manager->Decode(cipher_manager->Encode(s)));
  }

  TEST(CipherTests, CipherReadTest) {
    std::vector<BYTE> encryption_test;

    for (int i = 0; i < TABLE_SIZE; i++) {
      encryption_test.push_back(static_cast<BYTE>(i));
    }
    
    std::random_shuffle(encryption_test.begin(), encryption_test.end());

    Base64 base64er;
    std::vector<BYTE> encoded_passwd = base64er.base64_encode(encryption_test);
    // EXPECT_EQ(base64er.base64_decode(std::string(encoded_passwd.begin(), encoded_passwd.end())), encryption_test);
    auto cipher_manager = Cipher::Instance(std::string(encoded_passwd.begin(), encoded_passwd.end()));
    EXPECT_EQ(cipher_manager->GetEncryptionTable(), encryption_test);

    std::string test_str = "Hello, worldssdasdsadas";
    std::string example_encoded_str = cipher_manager->Decode(test_str);

    for (auto &s: example_encoded_str) {
      printf("%d\n", static_cast<uint8_t>(s));
    }
    std::cout << "<<<<\n";
    
    for (auto &s: test_str) {
      printf("%d\n", static_cast<uint8_t>(s));
      s = encryption_test[static_cast<uint8_t>(s)]; 
    }
    
    EXPECT_EQ(true, test_str == example_encoded_str);
  }
}
