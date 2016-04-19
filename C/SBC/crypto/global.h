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
#define MAX_MESSAGE_LENGTH 65535 /* max message length in bytes. NOT BIGGER THAN 2^16-1 (stored in short)*/

// RSA parameters, ALSO CHANGE global.c if you change these!!!
#define MODULUS_SIZE_MASTER 156 //see MAX_MODULUS_SIZE
#define MODULUS_SIZE_SLAVE 156
#define PUBLIC_SIZE_MASTER 3
#define PUBLIC_SIZE_SLAVE 3
#define PRIVATE_SIZE_MASTER 156
#define PRIVATE_SIZE_SLAVE 156
#define PRIME_SIZE 156
#define GENERATOR_SIZE 156

#define DH_PARAMETERS 20
#define MAX_MODULUS_SIZE 156
#define MAX_PRIME_SIZE 156

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


extern unsigned char MODULUS_MASTER[MODULUS_SIZE_MASTER];
extern unsigned char PUBLIC_EXPONENT_MASTER[PUBLIC_SIZE_MASTER];
extern unsigned char PRIVATE_EXPONENT_MASTER[PRIVATE_SIZE_MASTER];
extern unsigned char MODULUS_SLAVE[MODULUS_SIZE_SLAVE];
extern unsigned char PUBLIC_EXPONENT_SLAVE[PUBLIC_SIZE_SLAVE];
extern unsigned char PRIVATE_EXPONENT_SLAVE[PRIVATE_SIZE_SLAVE];
extern unsigned char PRIME[PRIME_SIZE];
extern unsigned char GENERATOR[PRIME_SIZE];


struct RSA_ctx_st
{
	unsigned char nprime_master[1];
	unsigned char nprime_slave[1];
	unsigned char nprime_prime[1];

    unsigned char rmodn_master[MODULUS_SIZE_MASTER];
    unsigned char rmodn_slave[MODULUS_SIZE_SLAVE];
    unsigned char rmodn_prime[PRIME_SIZE];

    unsigned char r2modn_master[MODULUS_SIZE_MASTER];
    unsigned char r2modn_slave[MODULUS_SIZE_SLAVE];
    unsigned char r2modn_prime[PRIME_SIZE];
};

typedef struct RSA_ctx_st RSA_ctx;

struct STS_ctx_st
{
	unsigned char private[DH_PARAMETERS];
    unsigned char base[PRIME_SIZE];
    unsigned char check[PRIME_SIZE];
};

typedef struct STS_ctx_st STS_ctx;

struct ENC_ctx_st
{
	unsigned char nonce[NONCE_LENGTH];
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
    unsigned char data[MAX_MESSAGE_LENGTH];
};

typedef struct message_ctx_st message_ctx;

#endif
