/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * counter_block.h
 *
 * Implementation of generator functions for counter
 * blocks used in CCM mode in AES encryption
 */

#ifndef __COUNTER_BLOCK_H
#define __COUNTER_BLOCK_H

void create_counter_block(unsigned char *out, const unsigned char *Nonce, const unsigned int m);

void create_cyphered_counter_block(unsigned char *out, const aes_key key, const unsigned char *Nonce, const unsigned int m);

#endif
