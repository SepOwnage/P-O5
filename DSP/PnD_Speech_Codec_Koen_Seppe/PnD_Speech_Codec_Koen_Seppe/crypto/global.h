/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * global.h
 *
 * This file contains declaration of global variables
 */
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "aes.h"

#define NONCE_LENGTH 8 /* Nonce length in bytes */
#define BLOCK_LENGTH 16 /* block length in bytes */
#define MAC_LENGTH 8 /* MAC length in bytes */
#define AES_KEY_SIZE 16 /* aes key length in bytes */
#define MAX_MESSAGE_LENGTH 645 /* max message length in bytes. NOT BIGGER THAN 2^16-1 (stored in short)*/

// RSA parameters, ALSO CHANGE global.c if you change these!!!
#define MODULUS_SIZE_MASTER 156/2 //see MAX_MODULUS_SIZE
#define MODULUS_SIZE_SLAVE 156/2
#define PUBLIC_SIZE_MASTER (3/2+1)
#define PUBLIC_SIZE_SLAVE (3/2+1)
#define PRIVATE_SIZE_MASTER 156/2
#define PRIVATE_SIZE_SLAVE 156/2
#define PRIME_SIZE 156/2
#define GENERATOR_SIZE 156/2

#define DH_PARAMETERS 20/2
#define MAX_MODULUS_SIZE 156/2
#define MAX_PRIME_SIZE 156/2

//choose between 3 collections of parameters (0 are the parameters on toledo, 1 and 2 are different, self-generated parameters
#ifndef SEL
#define SEL 0
#endif

#ifndef DEBUGL2
#define DEBUGL2 0
#endif
#ifndef DEBUGL1
#define DEBUGL1 0
#endif
#ifndef PRODUCTION_TEST
#define PRODUCTION_TEST 0
#endif

#if DEBUGL2 == 1
#define PRINT_messages 1
#define PRINT_errors 1
#define PRINT_inputs_outputs 1
#define TIMING 1
#elif DEBUGL1 == 1
#define PRINT_messages 0
#define PRINT_errors 1
#define PRINT_inputs_outputs 1
#define TIMING 1
#elif PRODUCTION_TEST == 1
#define PRINT_messages 0
#define PRINT_errors 0
#define PRINT_inputs_outputs 0
#define TIMING 1
#else
#define PRINT_messages 0
#define PRINT_errors 0
#define PRINT_inputs_outputs 0
#define TIMING 0
#endif


extern unsigned short MODULUS_MASTER[MODULUS_SIZE_MASTER];
extern unsigned short PUBLIC_EXPONENT_MASTER[PUBLIC_SIZE_MASTER];
extern unsigned short PRIVATE_EXPONENT_MASTER[PRIVATE_SIZE_MASTER];
extern unsigned short MODULUS_SLAVE[MODULUS_SIZE_SLAVE];
extern unsigned short PUBLIC_EXPONENT_SLAVE[PUBLIC_SIZE_SLAVE];
extern unsigned short PRIVATE_EXPONENT_SLAVE[PRIVATE_SIZE_SLAVE];
extern unsigned short PRIME[PRIME_SIZE];
extern unsigned short GENERATOR[PRIME_SIZE];


struct RSA_ctx_st
{
	unsigned short nprime_master[1];
	unsigned short nprime_slave[1];
	unsigned short nprime_prime[1];

    unsigned short rmodn_master[MODULUS_SIZE_MASTER];
    unsigned short rmodn_slave[MODULUS_SIZE_SLAVE];
    unsigned short rmodn_prime[PRIME_SIZE];

    unsigned short r2modn_master[MODULUS_SIZE_MASTER];
    unsigned short r2modn_slave[MODULUS_SIZE_SLAVE];
    unsigned short r2modn_prime[PRIME_SIZE];
};

typedef struct RSA_ctx_st RSA_ctx;

struct STS_ctx_st
{
	unsigned short private[DH_PARAMETERS];
    unsigned short base[PRIME_SIZE];
    unsigned short check[PRIME_SIZE];
};

typedef struct STS_ctx_st STS_ctx;

struct ENC_ctx_st
{
	unsigned short nonce[NONCE_LENGTH/2];
    unsigned int counter;
    aes_key key;
};

typedef struct ENC_ctx_st ENC_ctx;

//structure of the messages send through the channel
//tag can be:
//0x00: First message of STS_protocol (master)
//0x01: Second message of STS_protocol (slave)
//0x02: third message of STS_protocol (master)
//0x03: 4th message of STS_protocol (slave)
//0x04: 5th message of STS_protocol (master)
//0x05: optimal 6th message of STS_protocol (slave)
//0x10: message containing enciphered data
//0xFF: protocol failed

struct message_ctx_st
{
	unsigned char tag;
	unsigned int seq_number; //associated data
	unsigned short length; //in bytes. Message can maximally contain 256^4 bytes of data
    unsigned short data[MAX_MESSAGE_LENGTH];
};

typedef struct message_ctx_st message_ctx;

#endif
