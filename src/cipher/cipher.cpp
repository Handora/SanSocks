#include "cipher/cipher.h"
#include <memory>

namespace sansocks
{
	std::shared_ptr<Cipher> Cipher::single_cipher_ = nullptr;
	Cipher::Cipher()
	{
		base64_helper_ = std::make_shared<Base64>();
		int i = 0;
		i = 0;
		for (int i = 0; i < 128; i++)
			encryption_[i] = i;
		std::random_shuffle(encryption_.begin(), encryption_.end());
		for (int i = 0; i < 128; i++)
			decryption_[encryption_[i]] = i;
	}

	std::string Cipher::encode(std::string wait_encode)
	{
		for (auto &ele : wait_encode)
			ele = encryption_[ele];
		std::string ans;
		ans = base64_helper_->base64_encode(wait_encode);
		return std::move(ans);
	}

	std::string Cipher::decode(std::string wait_decode)
	{
		std::string ans;
		ans = base64_helper_->base64_decode(wait_decode);
		for (auto &ele : ans)
			ele = decryption_[ele];
		return std::move(ans);
	}
}


  int main(int argc, char *argv[])
  {
    using namespace sansocks;
    auto p = Cipher::Instance();
  std::cout << p->decode(p->encode("Hello"));
  system("pause");
  return 0;
}

