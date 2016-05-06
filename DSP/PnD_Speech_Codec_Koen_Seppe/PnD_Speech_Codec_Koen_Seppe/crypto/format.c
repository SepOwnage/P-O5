/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * format.c
 *
 * This implements a formatting function used in CCM mode
 * in AES
 */

#include <stdio.h>
#include <string.h>
#include "format.h"
#include "global.h"
#include "util.h"

/*
 * Generate formatted block in the patern of the NIST CCM recommendation
 * from a given nonce and input with given length. The bytes 4-7 from
 * the nonce are also used as assiciated data
 *
 * out          output array with size >= (length + 2*BLOCK_SIZE) rounded up to multiple of BLOCK_SIZE
 * nonce 		the nonce used to generate the formatted blocks
                the last 4 bytes are equal to the sequence number, which is used as associated data
 * in 			the input data used to generate the formatted blocks
 * length 		the length of the input data
 */
 void formatData(void *outin, const void *noncein ,const void *inin, unsigned int length)
 {
	unsigned char *out = outin;
	const unsigned char *nonce = noncein;
	const unsigned char *in = inin;

 	unsigned char t = MAC_LENGTH;
 	unsigned char q = 15-NONCE_LENGTH;
 	unsigned char flags = 64 + (((t-2)/2) << 3) + (q-1);
    unsigned int dataBlocks = (length-1)/BLOCK_LENGTH + 1;
    unsigned int i;

 	// create first block
    memset(out, 0x00, BLOCK_LENGTH);
 	*out = flags;
 	memcpy(out + 1, nonce, NONCE_LENGTH);
 	reverse_memcpy(out + BLOCK_LENGTH -4, &length, 4);

 	// create block with counter (associated data of size 4)
 	memset(out + BLOCK_LENGTH, 0x00,1);
 	memset(out + BLOCK_LENGTH+1, 0x04,1);
 	memcpy(out + BLOCK_LENGTH+2, nonce+NONCE_LENGTH-4, 4);
 	memset(out + BLOCK_LENGTH+6, 0x00, BLOCK_LENGTH-4-1-1);

 	// create data blocks (not the last)

 	for (i = 1; i < dataBlocks; i++) {
 		memcpy(out + BLOCK_LENGTH * (i+1), in + BLOCK_LENGTH * (i-1), BLOCK_LENGTH);
 	}

  	// create last datablock and pad data if needed
 	for (i = BLOCK_LENGTH * dataBlocks; i < BLOCK_LENGTH * (dataBlocks+1); i++){
 		out[i+BLOCK_LENGTH] = (i-BLOCK_LENGTH) < length ? in[i-BLOCK_LENGTH] : 0x00;
 	}
 }
