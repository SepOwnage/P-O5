/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * counter_block.c
 *
 * Implementation of generator functions for counter
 * blocks used in CCM mode in AES encryption
 */

# include <string.h>
# include "aes.h"
# include "util.h"
# include "global.h"

/*
 * Creates counter blocks of size BLOCK_LENGTH, as defined in NIST CCM mode with the given Nonce.
 *
 * out          output array with size >= m*BLOCK_SIZE
 * Nonce        the nonce used to generate the counter blocks
 * m            the amount of counter blocks to generate
 */
void create_counter_block(unsigned char *out, const unsigned char *Nonce, const unsigned int m)
{
    unsigned char flag;
    unsigned int counter;

    flag = NONCE_LENGTH;

    for (counter=0; counter != m; counter++){
        memset(out+BLOCK_LENGTH*counter, 0x00, BLOCK_LENGTH);
        memcpy(out+BLOCK_LENGTH*counter, &flag, 1);
        reverse_memcpy(out+12+BLOCK_LENGTH*counter, &counter, 4);
        memcpy(out+1+BLOCK_LENGTH*counter, Nonce, NONCE_LENGTH);
    };


}

/*
 * Creates cyphered counter blocks of size BLOCK_LENGTH, as defined in NIST CCM mode with given key and nonce.
 *
 * out          output array with size >= m*BLOCK_SIZE
 * Nonce        the nonce used to generate the counter blocks
 * key          the key to use in the AES encryption
 * m            the amount of counter blocks to generate
 */

void create_cyphered_counter_block(unsigned char *out, const aes_key key, const unsigned char *Nonce, const unsigned int m)
{
    unsigned char flag;
    unsigned int counter;
    unsigned char counter_block[BLOCK_LENGTH];

    flag = NONCE_LENGTH;

    for (counter=0; counter<m; counter++){
        memset(counter_block, 0x00, BLOCK_LENGTH);
        memcpy(counter_block, &flag, 1);
        reverse_memcpy(counter_block+(BLOCK_LENGTH-4), &counter, 4);
        memcpy(counter_block+1, Nonce, NONCE_LENGTH);
        aes_encrypt(&key, counter_block, out+BLOCK_LENGTH*counter);
    };
}
