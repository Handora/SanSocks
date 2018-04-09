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
		Cipher(std::vector<BYTE>);
		std::string Encode(const std::string&);
		std::string Decode(const std::string&);
		std::vector<BYTE> ToString();
		static std::shared_ptr<Cipher> Instance(const std::vector < BYTE >& passwd = {}) {
      if (single_cipher_ == nullptr)
        single_cipher_ = std::make_shared<Cipher>(passwd);
      return single_cipher_;
    }
    static std::shared_ptr<Cipher> single_cipher_;
    
  private: 
    std::vector<BYTE> encryption_;
    std::vector<BYTE> decryption_;
    std::shared_ptr<Base64> base64_helper_;
	std::vector<BYTE> Encode(const std::vector<BYTE>&);
	std::vector<BYTE> Decode(const std::vector<BYTE>&);
  }; 
};

