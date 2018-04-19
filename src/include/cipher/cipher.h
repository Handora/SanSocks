#pragma once
#include<vector>
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
    std::string Encode(const std::string&);
    std::string Decode(const std::string&);
    std::vector<BYTE> ToString();
    std::vector<BYTE> GetEncryptionTable() { return encryption_; };
    std::vector<BYTE> GetDecryptionTable() { return decryption_; };
    
    static std::shared_ptr<Cipher> Instance(const std::string& passwd = "") {
      if (single_cipher_ == nullptr)
        single_cipher_ = std::make_shared<Cipher>(passwd);
      return single_cipher_;
    }
    
    static std::shared_ptr<Cipher> single_cipher_;
  
  private:
    std::vector<BYTE> Encode(const std::vector<BYTE>&);
    std::vector<BYTE> Decode(const std::vector<BYTE>&);
  
    std::vector<BYTE> encryption_;
    std::vector<BYTE> decryption_;
    std::shared_ptr<Base64> base64_helper_;
  }; 
};

