/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * ISAAC.h
 *
 * A header for ISAAC to be able to include ISAAC
 */

#ifndef __ISAAC_H
#define __ISAAC_H


/* a ub4 is an unsigned 4-byte quantity */
typedef  unsigned long int  ub4;

/* external results */
extern ub4 randrsl[256], randcnt;

void isaac();

void randinit(int flag);

#endif
