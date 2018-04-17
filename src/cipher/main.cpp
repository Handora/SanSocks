#include "cipher/cipher.h"

int main(int, char *[])
{
  using namespace sansocks;
  auto cipherer = Cipher::Instance();
  std::cerr << "The Cipher is " << std::endl;
  auto cipher = cipherer->ToString();
  std::cout << std::string(cipher.begin(), cipher.end());
  std::cerr << std::endl;
  return 0;
}


