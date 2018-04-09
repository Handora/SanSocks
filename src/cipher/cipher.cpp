#include "cipher/cipher.h"
#include <algorithm>
#include <cassert>

namespace sansocks
{
  std::shared_ptr<Cipher> Cipher::single_cipher_ = nullptr;
  Cipher::Cipher(std::string passwd)
  {
    base64_helper_ = std::make_shared<Base64>(); 

    if (passwd == "") {
      // if the args is empty, we just generate a new cipher
      for (int i = 0; i < TABLE_SIZE; i++) 
	encryption_[i] = i;
    
      std::random_shuffle(encryption_.begin(), encryption_.end());
    } else {
      // else we generate cipher based on input string =passwd=
      std::string decoded_passwd = base64_helper_->base64_decode(passwd);
      // TODO(Handora): enhance the performance of copy => move
      // can this suceessful?
      std::move(decoded_passwd);
      std::copy(decoded_passwd.begin(), decoded_passwd.end(), encryption_.begin());
    }
    
    for (int i = 0; i < TABLE_SIZE; i++)
      decryption_[(int)reinterpret_cast<uint8_t>(encryption_[i])] = i; 
  }
  
  std::string Cipher::Encode(std::string wait_encode)
  {
    for (auto &ele : wait_encode)
      ele = encryption_[ele];
    std::string ans;
    ans = base64_helper_->base64_encode(wait_encode);
    return std::move(ans);
  }

  std::string Cipher::Decode(std::string wait_decode)
  {
    std::string ans;
    ans = base64_helper_->base64_decode(wait_decode);
    for (auto &ele : ans)
      ele = decryption_[ele];
    return std::move(ans);
  }

  std::string Cipher::ToString()
  {
    return base64_helper_->base64_encode(std::string(encryption_.begin(),encryption_.end())); 
  }
  
}

//int main(int, char**) {
//  using namespace sansocks;
//  auto cipher = Cipher::Instance();
//  std::cerr << "Generating a cipher:\n";
//  std::cout << cipher->ToString();
//  std::cerr << std::endl;
//  return 0;
//}



