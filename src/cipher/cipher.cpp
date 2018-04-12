#include "cipher/cipher.h"
#include <algorithm>
#include <cassert>

namespace sansocks
{
	std::shared_ptr<Cipher> Cipher::single_cipher_ = nullptr;
	Cipher::Cipher(std::string pwd)
	{
		base64_helper_ = std::make_shared<Base64>();
		std::vector<BYTE> passwd(pwd.begin(),pwd.end());
		if (passwd.empty()) {
			// if the args is empty, we just generate a new cipher
			encryption_.resize(TABLE_SIZE);
			for (int i = 0; i < TABLE_SIZE; i++)
				encryption_[i] = i;
			
			std::random_shuffle(encryption_.begin(), encryption_.end());
		}
		else {
			// else we generate cipher based on input string =passwd=
			encryption_.resize(TABLE_SIZE);
			std::vector<BYTE> decoded_passwd = base64_helper_->base64_decode(passwd);
			// TODO(Handora): enhance the performance of copy => move
			// can this suceessful?
			std::move(decoded_passwd);
			std::copy(decoded_passwd.begin(), decoded_passwd.end(), encryption_.begin());
		}
		decryption_.resize(TABLE_SIZE);
		for (int i = 0; i < TABLE_SIZE; i++)
			decryption_[(int)encryption_[i]] = i;
	}
	
	std::string Cipher::Encode(const std::string& wait_encode)
	{
		std::vector<BYTE> transform(wait_encode.begin(), wait_encode.end());
		transform = Encode(transform);
		std::move(transform);
		return std::move(std::string(transform.begin(), transform.end()));
	}

	std::vector<BYTE> Cipher::Encode(const std::vector<BYTE>& wait_encode)
	{
		std::vector<BYTE> ans(wait_encode.begin(), wait_encode.end());
		for (auto &ele : ans)
			ele = encryption_[ele];
		return std::move(ans);
	}

	std::string Cipher::Decode(const std::string& wait_decode)
	{
		std::vector<BYTE> transform(wait_decode.begin(), wait_decode.end());
		transform = Decode(transform);
		std::move(transform);
		return std::move(std::string(transform.begin(), transform.end()));
	}

	std::vector<BYTE> Cipher::Decode(const std::vector<BYTE>& wait_decode)
	{
		std::vector<BYTE> ans(wait_decode.begin(), wait_decode.end());
		for (auto &ele : ans)
			ele = decryption_[ele];
		return std::move(ans);
	}

  std::vector<BYTE> Cipher::ToString()
  {
    return base64_helper_->base64_encode(encryption_);
  }
}

// int main(int, char**) {
// using namespace sansocks;
// std::vector<BYTE> encryption_test;
// std::cerr << "test encode" << std::endl;
// encryption_test.resize(TABLE_SIZE);
// for (int i = 0; i < TABLE_SIZE; i++)
// 	  encryption_test[i] = i;
// std::random_shuffle(encryption_test.begin(), encryption_test.end());
// Base64 base64er;
// std::vector<BYTE> encoded_passwd = base64er.base64_encode(encryption_test);
// std::string pswd(encoded_passwd.begin(), encoded_passwd.end());
// auto cipher = Cipher::Instance(pswd);
// std::string test_str = "hello, world";
// std::string example_encoded_str = cipher->Encode(test_str);
// std::cout << cipher->Decode(example_encoded_str) << std::endl;
// std::cout << test_str << std::endl;
// for (auto& s : test_str)
// {
//     s = encryption_test[s];
// }
// std::string example = cipher->Decode(example_encoded_str);
// std::cout << std::boolalpha;
// std::cout << "length equal : " << (example_encoded_str.size() == test_str.size()) << std::endl;
// for (int i = 0; i < test_str.size(); i++)
// {
// 	  std::cout << "pos : " << i << " " << test_str[i] << " " << example_encoded_str[i] << " " << (test_str[i] == example_encoded_str[i]) << std::endl;
// }
// std::cout << (test_str == example_encoded_str) << std::endl;
// std::cerr << "test overflow" << std::endl;
// std::string overflow_string;
// for (int i = 0; i < 128; i++)
//   overflow_string.push_back(i + 127);
// std::string encode_overflow_string = cipher->Encode(overflow_string);
// for (auto& s : overflow_string)
//   s = encryption_test[(BYTE)s];
// std::cout << "test result : " << (overflow_string == encode_overflow_string) << std::endl;
// system("pause");
// return 0;

//  using namespace sansocks;
//   std::vector<BYTE> encryption_test;

//   encryption_test.resize(TABLE_SIZE);
//   for (int i = 0; i < TABLE_SIZE; i++) 
//     encryption_test[i] = i;
    
//   std::random_shuffle(encryption_test.begin(), encryption_test.end());

// Base64 base64er;
// std::vector<BYTE> encoded_passwd = base64er.base64_encode(encryption_test);
// auto cipher_manager = Cipher::Instance(std::string(encoded_passwd.begin(), encoded_passwd.end()));

// std::string test_str = "Hello, worldssdasdsadas";
// std::string example_encoded_str = cipher_manager->Encode(test_str); 
// for (auto &s: test_str) {
//   s = encryption_test[(BYTE)s];
//   std::cout << (int)s << std::endl;
// }
    
// std::cout << (test_str ==  example_encoded_str);
// }



