#pragma once

#include <algorithm>
#include <string>
#include <memory>
#include <array>

#include "cipher/base64.h"
#include "utility/util.h"

namespace sansocks
{
  // TODO(Tanglun): change char to uint8_t 
  
  class Cipher
  {
  public:
    Cipher(std::string); 
    std::string Encode(std::string);
    std::string Decode(std::string);
    std::string ToString();
    static std::shared_ptr<Cipher> Instance(std::string passwd="") {
      if (single_cipher_ == nullptr)
        single_cipher_ = std::make_shared<Cipher>(passwd);
      return single_cipher_;
    }
    static std::shared_ptr<Cipher> single_cipher_;
    
  private: 
    std::array<BYTE, TABLE_SIZE> encryption_;
    std::array<BYTE, TABLE_SIZE> decryption_;
    std::shared_ptr<Base64> base64_helper_;
  }; 
};

