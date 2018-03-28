#pragma once

#include <algorithm>
#include <string>
#include <memory>
#include <array>
#include "base64.h"

namespace cipher
{
  class Cipher
  {
  public:
    Cipher();
    std::string encode(std::string);
    std::string decode(std::string);
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

