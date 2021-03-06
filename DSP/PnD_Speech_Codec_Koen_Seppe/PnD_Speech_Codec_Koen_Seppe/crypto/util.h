/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * util.h
 *
 * This file contains declaration of utility functions
 */

#ifndef __UTIL_H
#define __UTIL_H

#include "global.h"

 void *reverse_memcpy(void *dst, const void *src, size_t len);

 void *memset_short(void *dst, int c, size_t n);

 void print_hex_memory(const void *mem, int length);

 void print_hex_memory_short(const void *mem, int length);

 void XOR(void *out, const void *in1, const void *in2, const unsigned int XORlen);

 void transform_char_to_short(unsigned short *out, const unsigned char *in1, int length);

 void print_message(const message_ctx *message);

 void convert_raw_message_to_message(message_ctx *out, const unsigned char *in);

 void convert_message_to_raw_message(unsigned char *out, const message_ctx *in);


#endif
