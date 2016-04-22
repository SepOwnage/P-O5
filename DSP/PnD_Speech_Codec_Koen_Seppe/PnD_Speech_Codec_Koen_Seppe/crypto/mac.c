/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * mac.c
 *
 * This implements everything for the MAC generation used in CCM mode
 * in AES
 */

#include <string.h>
#include "global.h"
#include "util.h"
#include "aes.h"

/*
 * Generate a MAC based on the given blocks and AES encryption with
 * the given key as described in the NIST CCM recomendation.
 *
 * out          	output array with size >= MAC_LENGTH
 * key 				the AES key used in the encryption
 * blocks			the blocks used to generate the MAC
 * length 			the length of the input block in the amount of blocks
 */
void generateMAC(unsigned char *out, const aes_key key, const unsigned char *blocks, const unsigned int length) {

	unsigned char temp[BLOCK_LENGTH];

	aes_encrypt(&key, blocks, temp);

	int i;
	for(i = 1; i<length;i++) {

		XOR(temp, temp, blocks + i*BLOCK_LENGTH, BLOCK_LENGTH);
		aes_encrypt(&key, temp, temp);
	}
	memcpy(out, temp, MAC_LENGTH);
}
