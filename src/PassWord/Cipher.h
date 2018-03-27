#ifndef CIPHER
#define CIPHER
#include <algorithm>
#include <string>
#include <memory>
#include <array>
#include "Base64.h"

namespace cipher
{
  class Cipher
  {
  public:
    Cipher();
    std::string encode(std::string);
    std::string decode(std::string);
    static std::shared_ptr<Cipher> Instance();
  private:
    std::array<char, 256> encryption_;
    std::array<char, 256> decryption_;
    std::shared_ptr<Base64> base64_helper_;
    static std::shared_ptr<Cipher> single_cipher_;
  };
  std::shared_ptr<Cipher> Cipher :: single_cipher_ = std::make_shared<Cipher>();
  std::shared_ptr<Cipher> Cipher::Instance()
  {
    return single_cipher_;
  }
};

#endif