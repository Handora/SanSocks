#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
namespace sansocks
{
  static std::vector<char> encoding_table {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
      'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
      'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
      'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
      'w', 'x', 'y', 'z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '+', '/'};
  static std::vector<int> mod_table{0,2,1};
  class Base64
  {
  public:
    Base64();
    ~Base64(){};
    std::string base64_encode(const std::string& data);
    std::string base64_decode(const std::string& data);
  private: 
    void build_decoding_table();

    static std::vector<char> decoding_table_;
  };
}


