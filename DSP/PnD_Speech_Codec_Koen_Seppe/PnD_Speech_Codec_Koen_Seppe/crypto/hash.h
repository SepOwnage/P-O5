/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * hash.h
 *
 * A wrapper around the sha3 has function
 */
#ifndef __HASH_H
#define __HASH_H

void hash(void *outin, const unsigned int outputLength, const void *inin, const unsigned int inputLength);

#endif
