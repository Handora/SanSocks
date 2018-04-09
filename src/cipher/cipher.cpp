#include "cipher/cipher.h"
#include <algorithm>
#include <cassert>

namespace sansocks
{
	std::shared_ptr<Cipher> Cipher::single_cipher_ = nullptr;
	Cipher::Cipher(std::vector<BYTE> passwd)
	{
		base64_helper_ = std::make_shared<Base64>();

		if (passwd.empty()) {
			// if the args is empty, we just generate a new cipher
			encryption_.resize(TABLE_SIZE);
			for (int i = 0; i < TABLE_SIZE; i++)
				encryption_[i] = i;

			std::random_shuffle(encryption_.begin(), encryption_.end());
		}
		else {
			// else we generate cipher based on input string =passwd=
			std::vector<BYTE> decoded_passwd = base64_helper_->base64_decode(passwd);
			// TODO(Handora): enhance the performance of copy => move
			// can this suceessful?
			std::move(decoded_passwd);
			std::copy(decoded_passwd.begin(), decoded_passwd.end(), encryption_.begin());
		}
		decryption_.resize(TABLE_SIZE);
		for (int i = 0; i < TABLE_SIZE; i++)
			decryption_[encryption_[i]] = i;
	}

	std::string Cipher::Encode(const std::string& wait_encode)
	{
		std::vector<BYTE> transform(wait_encode.begin(), wait_encode.end()); 
		std::vector<BYTE> after_encode = Encode(transform);
		return std::move(std::string(after_encode.begin(), after_encode.end()));
	}

	std::string Cipher::Decode(const std::string& wait_decode)
	{
		std::vector<BYTE> transform(wait_decode.begin(), wait_decode.end());
		std::vector<BYTE> after_decode = Decode(transform);
		return std::move(std::string(after_decode.begin(), after_decode.end()));
	}

	std::vector<BYTE> Cipher::Encode(const std::vector<BYTE>& wait_encode)
	{
		std::vector<BYTE> ans(wait_encode.begin(), wait_encode.end());
		for (auto &ele : ans)
			ele = encryption_[ele];
		ans = base64_helper_->base64_encode(ans);
		return std::move(ans);
	}

	std::vector<BYTE> Cipher::Decode(const std::vector<BYTE>& wait_decode)
	{
		std::vector<BYTE> ans = base64_helper_->base64_decode(wait_decode);
		for (auto &ele : ans)
			ele = decryption_[ele];
		return std::move(ans);
	}

	std::vector<BYTE> Cipher::ToString()
	{
		return base64_helper_->base64_encode(encryption_);
	}

}

int main(int, char**) {
  using namespace sansocks;
  auto cipher = Cipher::Instance();
  std::string s;
  for (int i = 0; i < 10; i++)
	  s.push_back(-i);
  std::cerr << "generating a cipher:\n";
  std::cout << cipher->Decode(cipher->Encode("Hello world"));
  std::cerr << std::endl;
  std::cerr << "generating a cipher:\n";
  std::cout << std::boolalpha;
  std::cout << (cipher->Decode(cipher->Encode(s)) == s) << std::endl;
  system("pause");
  return 0;
}



