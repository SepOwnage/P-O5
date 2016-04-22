/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * format.h
 *
 * This implements a formatting function used in CCM mode
 * in AES
 */

#ifndef __FORMAT_H
#define __FORMAT_BLOCK_H

void formatData(unsigned char *out, const unsigned char *nonce ,const unsigned char *in, unsigned int length);

#endif
