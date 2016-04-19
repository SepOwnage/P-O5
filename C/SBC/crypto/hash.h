/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * hash.h
 *
 * A wrapper around the sha3 has function
 */
#ifndef __HASH_H
#define __HASH_H

 void hash(unsigned char *out, const unsigned int Outputlength, const unsigned char *in, const unsigned int inputLength);

#endif
