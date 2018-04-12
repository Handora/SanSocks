#include "cipher/base64.h"

namespace sansocks
{
	std::vector<BYTE> Base64::encoding_table = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/' };
	std::vector<BYTE> Base64::decoding_table_;
	std::vector<BYTE> Base64::base64_encode(const std::vector<BYTE>& data)
	{
		size_t input_length = data.size();
		size_t output_length = 4 * ((input_length + 2) / 3);

		std::vector<BYTE> encoded_data;
		encoded_data.resize(output_length);

		if (encoded_data.empty())
			return encoded_data;

		for (int i = 0, j = 0; i < input_length;)
		{

			uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
			uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
			uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

			uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

			encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
			encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
			encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
			encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
		}

		for (int i = 0; i < mod_table[input_length % 3]; i++)
			encoded_data[output_length - 1 - i] = '=';

		return std::move(encoded_data);
	}

	std::vector<BYTE> Base64::base64_decode(const std::vector<BYTE>& data)
	{
		size_t input_length = data.size();
		std::vector<BYTE> decoded_data;
		if (input_length % 4 != 0)
			return decoded_data;
		size_t len = data.size() / 4 * 3;
		size_t* output_length = &len;
		if (data[input_length - 1] == '=')
			(*output_length)--;
		if (data[input_length - 2] == '=')
			(*output_length)--;
		decoded_data.resize(*output_length);
		/*
		  if (decoded_data == NULL)
		  return NULL;
		*/
		for (int i = 0, j = 0; i < input_length;)
		{
			uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table_[data[i++]];
			uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table_[data[i++]];
			uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table_[data[i++]];
			uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table_[data[i++]];

			uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

			if (j < *output_length)
				decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
			if (j < *output_length)
				decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
			if (j < *output_length)
				decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
		}

		return std::move(decoded_data);
	}

	void Base64::build_decoding_table() {
		decoding_table_.clear();
		decoding_table_.resize(256);
		for (int i = 0; i < 64; i++)
			decoding_table_[(unsigned char)encoding_table[i]] = i;
	}

	Base64::Base64() {
		if (decoding_table_.empty()) {
			build_decoding_table();
		}
	}
}
