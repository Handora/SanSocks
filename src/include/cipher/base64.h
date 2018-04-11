#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "utility/util.h"
namespace sansocks
{
  static std::vector<int> mod_table{0,2,1};
  class Base64
  {
  public:
    Base64();
    ~Base64(){};
	std::vector<BYTE> base64_encode(const std::vector<BYTE>& data);
	std::vector<BYTE> base64_decode(const std::vector<BYTE>& data);
  private: 
    void build_decoding_table();

    static std::vector<BYTE> decoding_table_;
	static std::vector<BYTE> encoding_table;
  };
}


