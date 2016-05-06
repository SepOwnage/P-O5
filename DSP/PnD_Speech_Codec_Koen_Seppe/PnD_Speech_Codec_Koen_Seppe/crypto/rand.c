/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * rand.c
 *
 * A wrapper around the ISAAC random number generator
 */

#include "ISAAC.h"
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

int count;
int initRand = 0;

/*
 * Initialize the isaac CSPRNG with a seed
 */
void isaac_init(void)
{
	srand ( time(NULL) );
	randrsl[0] = rand(); //not secure, but good for now
	randrsl[1] = rand();
	randrsl[2] = rand();
	randrsl[3] = rand();
	randrsl[4] = rand();
	randrsl[5] = rand();

    randinit(1);
    isaac();
    count = 256;
}

/*
 * Generate a random number with the ISAAC CSPRNG of
 * given size
 *
 * out		output array of with size >= given size
 * size 	the size of the random number to generate in bits
 */
void isaac_rand(void *outin, const int size)
{   unsigned char *out=outin;
    int i;

    if(initRand !=1){
    	isaac_init();
    	initRand = 1;
    }

	for(i = 0; ((size)/32) > i; i++){

		if(count < 1){

			isaac();
			count = 256;
		}
		count--;
		memcpy(out+4*i, randrsl+count, 4);
	}
	if(size % 32 > 0){
		if(count < 1){
			isaac();
			count = 256;
		}
		count--;
		memcpy(out+4*i, randrsl+count, (size-1)/8+1);
	}
}
