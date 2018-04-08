#include "cipher/base64.h"

namespace cipher
{
	std::vector<char> Base64::decoding_table;
	std::string Base64::base64_encode(const std::string& data)
	{
		size_t input_length = data.size();
		size_t output_length = 4 * ((input_length + 2) / 3);

		std::string encoded_data;
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

	std::string Base64::base64_decode(const std::string& data)
	{
		size_t input_length = data.size();
		if (decoding_table.empty())
			build_decoding_table();
		std::string decoded_data;
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

			uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
			uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
			uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
			uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

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

		//decoding_table = new char[256];
		decoding_table.resize(256);
		for (int i = 0; i < 64; i++)
			decoding_table[(unsigned char)encoding_table[i]] = i;
	}


	void Base64::base64_cleanup() {
		decoding_table.clear();
	}
}
