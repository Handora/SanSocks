#ifndef BASE_64_H
#define BASE_64_H
#include <stdint.h>
#include <stdlib.h>
#include <iostream>

class base64
{
public:
  base64();
  ~base64();
  char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length);
  char *base64_decode(const char *data, size_t input_length, size_t *output_length);
private:
  static char *decoding_table = NULL;
  static int mod_table[] = {0, 2, 1};
}

#endif