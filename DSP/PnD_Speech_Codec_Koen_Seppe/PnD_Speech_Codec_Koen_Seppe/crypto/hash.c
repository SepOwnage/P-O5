/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * hash.c
 *
 * A wrapper around the sha3 has function
 */

#include "sha3.h"

int initHash = 0;
struct sha3_256_ctx ctx;

/*
 * Generate a hash from the given input with the sha3 algorithm with
 * a given outputLength
 *
 * out          	output array with size >= outputLength
 * outputLength		the length of the hash to be generated. (there are
 *					some restrictions on this length, but still need to
 *					figure out which ones.)
 * in 				the input data used to generate the hash from
 * inputLength 		the length of the input data
 */
void hash(void *outin, const unsigned int outputLength, const void *inin, const unsigned int inputLength)
{   unsigned char *out= outin;
    const unsigned char *in = inin;

    if(initHash != 1){
    	sha3_256_init(&ctx);
    	initHash = 1;
    }

    sha3_256_update(&ctx, inputLength, in);
	sha3_256_digest(&ctx, outputLength, out);
}
