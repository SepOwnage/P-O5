/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * mac.h
 *
 * This implements everything for the MAC generation used in CCM mode
 * in AES
 */

#ifndef __MAC_H
#define __MAC_H

void generateMAC(unsigned char *out, const aes_key key, const unsigned char *blocks, const unsigned int length);

#endif
