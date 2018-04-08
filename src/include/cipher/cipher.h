#pragma once

#include <algorithm>
#include <string>
#include <memory>
#include <array>
#include "cipher/base64.h"

namespace sansocks
{
  class Cipher
  {
  public:
    Cipher();
    std::string Encode(std::string);
    std::string Decode(std::string);
    std::string ToString();
    static std::shared_ptr<Cipher> Instance() {
      if (single_cipher_ == nullptr)
        single_cipher_ = std::make_shared<Cipher>();
      return single_cipher_;
    }
    static std::shared_ptr<Cipher> single_cipher_;
  private: 
    std::array<char, 128> encryption_;
    std::array<char, 128> decryption_;
    std::shared_ptr<Base64> base64_helper_;
  }; 
};

