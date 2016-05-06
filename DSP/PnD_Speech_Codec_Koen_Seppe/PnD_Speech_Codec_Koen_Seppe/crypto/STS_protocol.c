/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * STS_protocol.c
 *
 * Implementation of the STS-protocol messages and the needed RSA en DH parameters.
 */

#include <string.h>
#include "global.h"
#include "modular_arithmetic.h"
#include "aes.h"
#include "CCM.h"
#include "rand.h"
#include "hash.h"
#include "util.h"


/*
 * initialize the give RSA context with parameters
 * needed for modular arithmetic
 *
 * ctx      pointer to the RSA context to initialize
 */
void calculate_parameters_RSA(RSA_ctx *ctx)
{
    calculate_n_inv(MODULUS_MASTER[0],ctx->nprime_master);
    calculate_n_inv(MODULUS_SLAVE[0],ctx->nprime_slave);
    calculate_n_inv(PRIME[0],ctx->nprime_prime);


    calculate_rmodn(ctx->rmodn_master,MODULUS_MASTER, MODULUS_SIZE_MASTER);
    calculate_rmodn(ctx->rmodn_slave,MODULUS_SLAVE, MODULUS_SIZE_SLAVE);
    calculate_rmodn(ctx->rmodn_prime,PRIME, PRIME_SIZE);

    calculate_r2modn(ctx->r2modn_master,MODULUS_MASTER,MODULUS_SIZE_MASTER);
    calculate_r2modn(ctx->r2modn_slave,MODULUS_SLAVE, MODULUS_SIZE_SLAVE);
    calculate_r2modn(ctx->r2modn_prime,PRIME, PRIME_SIZE);
}

/*
 * generate the first message of the master for the STS
 * protocol and update the given master STS context.
 *
 * message          pointer to the message context to hold the generated message
 * STS_ctx_master   pointer to the master STS context
 */
void STS_message1_master(message_ctx *message, STS_ctx *STS_ctx_master){

    //generate c
    memcpy(STS_ctx_master->base, GENERATOR, PRIME_SIZE*2);

    //send c
    message->tag = 0x00;
    message->length = PRIME_SIZE*2;
    memcpy(message->data, STS_ctx_master->base, PRIME_SIZE*2);
}

/*
 * generate the first message of the slave for the STS
 * protocol and update the given slave STS context based on
 * the slave RSA context and input message.
 *
 * message          pointer to the message context to hold the generated message
 * STS_ctx_slave    pointer to the slave STS context
 * RSA_ctx_slave    pointer to the slave RSA context used to generate the message
 * input            pointer to the received input message
 */
void STS_message1_slave(message_ctx *message, STS_ctx *STS_ctx_slave, const RSA_ctx *RSA_ctx_slave, const message_ctx *input){

    //read c
    memcpy(STS_ctx_slave->base, input->data, PRIME_SIZE*2);


    //generate b calculate c^b mod p
    do{isaac_rand(STS_ctx_slave->private, DH_PARAMETERS*8*2);
    montgomery_exponentiation(STS_ctx_slave->check, STS_ctx_slave->base, STS_ctx_slave->private, DH_PARAMETERS, PRIME, PRIME_SIZE, RSA_ctx_slave->nprime_prime, RSA_ctx_slave->rmodn_prime, RSA_ctx_slave->r2modn_prime);
    }while (is_save_message(STS_ctx_slave->check, PRIME,PRIME_SIZE)==0);

    //send c^b mod p
    message->tag = 0x01;
    message->length = PRIME_SIZE*2;
    memcpy(message->data, STS_ctx_slave->check, PRIME_SIZE*2);
}

/*
 * generate the second message of the master for the STS
 * protocol, update the given master STS context and initialize
 * the given master encryption context based on the master RSA
 * context and input message.
 *
 * message          pointer to the message context to hold the generated message
 * ENC_ctx_master   pointer to the master encryption context to be initialized
 * RSA_ctx_master   pointer to the master RSA context used to generate the message
 * STS_ctx_master   pointer to the master STS context
 * input            pointer to the received input message
 */
void STS_message2_master(message_ctx *message, ENC_ctx *ENC_ctx_master, STS_ctx *STS_ctx_master, const RSA_ctx *RSA_ctx_master, const message_ctx *input){

    //calculate hash(c) as nonce and set counter part 0
    hash(ENC_ctx_master->nonce, NONCE_LENGTH, STS_ctx_master->base, PRIME_SIZE*2);
    unsigned int * temporary = (unsigned int *) (ENC_ctx_master->nonce+2);
    *temporary = ENC_ctx_master->counter;

    //generate a and calculate c^a mod p
    do{isaac_rand(STS_ctx_master->private, DH_PARAMETERS*8*2);
    montgomery_exponentiation(STS_ctx_master->check, STS_ctx_master->base, STS_ctx_master->private, DH_PARAMETERS, PRIME, PRIME_SIZE, RSA_ctx_master->nprime_prime, RSA_ctx_master->rmodn_prime, RSA_ctx_master->r2modn_prime);
    }while (is_save_message(STS_ctx_master->check, PRIME,PRIME_SIZE)==0);

    //read c^b mod p
    memcpy(STS_ctx_master->base, input->data, PRIME_SIZE*2);

    //send c^a mod p
    message->tag = 0x02;
    message->length = PRIME_SIZE*2;
    memcpy(message->data, STS_ctx_master->check, PRIME_SIZE*2);
}

/*
 * generate the second message of the slave for the STS
 * protocol, update the given slave STS context and initialize
 * the given slave encryption context based on the slave RSA
 * context and input message.
 *
 * message          pointer to the message context to hold the generated message
 * ENC_ctx_slave    pointer to the slave encryption context to be initialized
 * STS_ctx_slave    pointer to the slave STS context
 * RSA_ctx_slave    pointer to the slave RSA context used to generate the message
 * input            pointer to the received input message
 */
void STS_message2_slave(message_ctx *message, ENC_ctx *ENC_ctx_slave, STS_ctx *STS_ctx_slave, const RSA_ctx *RSA_ctx_slave, const message_ctx *input){

    unsigned short KEY[PRIME_SIZE];
    unsigned short TEMP[2*PRIME_SIZE];

    //calculate hash(c) and set counter part 0
    hash(ENC_ctx_slave->nonce, NONCE_LENGTH, STS_ctx_slave->base, PRIME_SIZE*2);
    unsigned int * temporary = (unsigned int *) (ENC_ctx_slave->nonce+2);
    *temporary = ENC_ctx_slave->counter;

    //read c^a mod p
    memcpy(STS_ctx_slave->base, input->data, PRIME_SIZE*2);

    //calculate c^a^b mod p
    montgomery_exponentiation(KEY, STS_ctx_slave->base, STS_ctx_slave->private, DH_PARAMETERS, PRIME, PRIME_SIZE, RSA_ctx_slave->nprime_prime, RSA_ctx_slave->rmodn_prime, RSA_ctx_slave->r2modn_prime);

    //hash(c^a^b mod p) -> key
    hash(KEY, AES_KEY_SIZE, KEY, PRIME_SIZE*2);
    aes_set_encrypt_key(&(ENC_ctx_slave->key),KEY, AES_KEY_SIZE*8);

    //compute hash(c^b|c^a)
    memcpy(TEMP, STS_ctx_slave->check, PRIME_SIZE*2);
    memcpy(TEMP+PRIME_SIZE, STS_ctx_slave->base, PRIME_SIZE*2);
    hash(KEY, MODULUS_SIZE_SLAVE*2, TEMP, 2*PRIME_SIZE*2);
    mod_reduction(KEY,KEY, MODULUS_SLAVE, MODULUS_SIZE_SLAVE);

    //sign, encrypt and send hash(c^b|c^a)
    montgomery_exponentiation(TEMP, KEY, PRIVATE_EXPONENT_SLAVE, PRIVATE_SIZE_SLAVE, MODULUS_SLAVE, MODULUS_SIZE_SLAVE, RSA_ctx_slave->nprime_slave, RSA_ctx_slave->rmodn_slave, RSA_ctx_slave->r2modn_slave);
    CCM_encrypt(message, ENC_ctx_slave, TEMP, MODULUS_SIZE_SLAVE*2);
    message->tag = 0x03;
}

/*
 * generate the third message of the master and a status for the STS
 * protocol and update the given master STS context
 * the given master encryption context based on the slave RSA
 * context and input message.
 *
 * message          pointer to the message context to hold the generated message
 * status           pointer to the output status
 * ENC_ctx_master   pointer to the master encryption context to be initialized
 * STS_ctx_master   pointer to the master STS context
 * RSA_ctx_master   pointer to the master RSA context used to generate the message
 * input            pointer to the received input message
 */
void STS_message3_master(message_ctx *message, unsigned char *status, ENC_ctx *ENC_ctx_master, STS_ctx *STS_ctx_master, const RSA_ctx *RSA_ctx_master, const message_ctx *input){

    unsigned short KEY[PRIME_SIZE];
    unsigned short TEMP[2*PRIME_SIZE];
    unsigned int decrypt_size;

    // calculate c^b^a
    montgomery_exponentiation(KEY, STS_ctx_master->base, STS_ctx_master->private, DH_PARAMETERS, PRIME, PRIME_SIZE, RSA_ctx_master->nprime_prime, RSA_ctx_master->rmodn_prime, RSA_ctx_master->r2modn_prime);

    //hash(c^b^a mod p) -> key
    hash(KEY, AES_KEY_SIZE, KEY, PRIME_SIZE*2);
    aes_set_encrypt_key(&(ENC_ctx_master->key),KEY, AES_KEY_SIZE*8);

    //decrypt input and design to compare
    CCM_decrypt(TEMP, &decrypt_size, ENC_ctx_master, input);
    montgomery_exponentiation(KEY, TEMP, PUBLIC_EXPONENT_SLAVE, PUBLIC_SIZE_SLAVE, MODULUS_SLAVE, MODULUS_SIZE_SLAVE, RSA_ctx_master->nprime_slave, RSA_ctx_master->rmodn_slave, RSA_ctx_master->r2modn_slave);

    //compute hash(c^b|c^a)
    memcpy(TEMP, STS_ctx_master->base, PRIME_SIZE*2);
    memcpy(TEMP+PRIME_SIZE, STS_ctx_master->check, PRIME_SIZE*2);
    hash(TEMP, MODULUS_SIZE_SLAVE*2, TEMP, 2*PRIME_SIZE*2);
    mod_reduction(TEMP,TEMP, MODULUS_SLAVE, MODULUS_SIZE_SLAVE);


    if (memcmp(KEY,TEMP,MODULUS_SIZE_SLAVE*2)!=0)
    {
        if (PRINT_errors){printf("\n protocol failed");}
       *status = 0;
       return;
    };

    //compute hash(c^a|c^b)
    memcpy(TEMP, STS_ctx_master->check, PRIME_SIZE*2);
    memcpy(TEMP+PRIME_SIZE, STS_ctx_master->base, PRIME_SIZE*2);
    hash(KEY, MODULUS_SIZE_MASTER*2, TEMP, 2*PRIME_SIZE*2);
    mod_reduction(KEY,KEY, MODULUS_MASTER, MODULUS_SIZE_MASTER);

    //sign, encrypt and send hash(c^a|c^b)
    montgomery_exponentiation(TEMP, KEY, PRIVATE_EXPONENT_MASTER, PRIVATE_SIZE_MASTER, MODULUS_MASTER, MODULUS_SIZE_MASTER, RSA_ctx_master->nprime_master, RSA_ctx_master->rmodn_master, RSA_ctx_master->r2modn_master);

    CCM_encrypt(message, ENC_ctx_master, TEMP, MODULUS_SIZE_MASTER*2);
    message->tag = 0x04;

}

/*
 * generate the third message of the slave for the STS
 * protocol and update the given slave STS context
 * the given slave encryption context based on the slave RSA
 * context and input message.
 *
 * message          pointer to the message context to hold the generated message
 * ENC_ctx_slave    pointer to the slave encryption context to be initialized
 * STS_ctx_slave    pointer to the slave STS context
 * RSA_ctx_slave    pointer to the slave RSA context used to generate the message
 * input            pointer to the received input message
 */
void STS_message3_slave(message_ctx *message, ENC_ctx *ENC_ctx_slave, STS_ctx *STS_ctx_slave, const RSA_ctx *RSA_ctx_slave, const message_ctx *input){


    unsigned short KEY[PRIME_SIZE];
    unsigned short TEMP[2*PRIME_SIZE];
    unsigned int decrypt_size;

    //decrypt input and design to compare

    CCM_decrypt(TEMP, &decrypt_size, ENC_ctx_slave, input);
    montgomery_exponentiation(KEY, TEMP, PUBLIC_EXPONENT_MASTER, PUBLIC_SIZE_MASTER, MODULUS_MASTER, MODULUS_SIZE_MASTER, RSA_ctx_slave->nprime_master, RSA_ctx_slave->rmodn_master, RSA_ctx_slave->r2modn_master);

   //compute hash(c^a|c^b)
    memcpy(TEMP, STS_ctx_slave->base, PRIME_SIZE*2);
    memcpy(TEMP+PRIME_SIZE, STS_ctx_slave->check, PRIME_SIZE*2);
    hash(TEMP, MODULUS_SIZE_MASTER*2, TEMP, 2*PRIME_SIZE*2);
    mod_reduction(TEMP,TEMP, MODULUS_MASTER, MODULUS_SIZE_MASTER);


    if (memcmp(KEY,TEMP,MODULUS_SIZE_MASTER*2)!=0)
    {
        message->tag = 0x05;
        message->length = 0;
    };

}
