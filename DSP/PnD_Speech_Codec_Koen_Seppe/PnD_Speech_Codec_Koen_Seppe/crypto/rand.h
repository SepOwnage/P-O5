/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * rand.h
 *
 * A wrapper around the ISAAC random number generator
 */

#ifndef __RAND_H
#define __RAND_H

 void isaac_init(void);
 void isaac_rand(void *outin, const int size);

#endif
