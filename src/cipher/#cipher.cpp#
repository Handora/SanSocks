#include "cipher.h"
#include <memory>

namespace cipher
{ 
  Cipher ::Cipher()
  {
    base64_helper_ = std::make_shared<Base64>();
    int i = 0;
    i = 0;
	for (int i = 0; i < 128; i++)
	{
		encryption_[i] = i;
	}
    std::random_shuffle(encryption_.begin(), encryption_.end());
	for (int i = 0; i < 128; i++)
		std::cout << i << " " << (int)encryption_[i] << std::endl;
    for(int i = 0;i < 128;i++)
      decryption_[encryption_[i]] = i;
  }

  std::string Cipher ::encode(std::string wait_encode)
  {
	  std::cout << "in encode" << std::endl;
	  std::cout << wait_encode << std::endl;
    for (auto &ele : wait_encode)
      ele = encryption_[ele];
	std::cout << wait_encode << std::endl;
    size_t out_sz = 0;
    char *after_encode = nullptr;
    after_encode = base64_helper_->base64_encode(reinterpret_cast<const unsigned char*> (wait_encode.c_str()), static_cast<size_t>(wait_encode.size()), &out_sz);
    std::string ans(after_encode, after_encode + out_sz);
    delete after_encode;
    return std::move(ans);
  }

  std::string Cipher ::decode(std::string wait_decode)
  {
    
    size_t out_sz = 0;
    unsigned char *after_decode = nullptr;
    after_decode = base64_helper_->base64_decode(wait_decode.c_str(), static_cast<size_t>(wait_decode.size()), &out_sz);
    std::string ans(after_decode, after_decode + out_sz);
	for (auto &ele : ans)
      ele = decryption_[ele];
	delete after_decode;
    return std::move(ans);
  }
  
}
int main(int argc, char *argv[])
{
  using namespace cipher;
  auto p = Cipher::Instance();
  std::string s = p->encode("Hello");
  std::cout << p->decode(s);
  system("pause");
  return 0;
}
