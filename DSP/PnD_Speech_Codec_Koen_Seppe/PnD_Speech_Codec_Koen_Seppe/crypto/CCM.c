/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * CCM_encrypt.c
 *
 * Implementation of a CCM encryption and decryption function
 * that can be used in AES
 */

#include <string.h>
#include "aes.h"
#include "format.h"
#include "global.h"
#include "util.h"
#include "counter_block.h"
#include "mac.h"


 /*
 * Encrypt the given Payload, update sender context and output
 * decryption in ciphermessage.
 *
 * ciphermessage        pointer output array with size >= payload + MAC_LENGTH
 * ENC_sender_ctx       pointer to the encoding sender context
 * Payload              pointer to the plaintext to send
 * Plen                 the length of Payload in bytes max size = MAX_MESSAGE_LENGTH - MAC_LENGTH
 */
 void CCM_encrypt(message_ctx *ciphermessage, ENC_ctx *ENC_sender_ctx, const unsigned char *Payload, const unsigned int Plen)
 {
    unsigned char TEMP[((MAX_MESSAGE_LENGTH - MAC_LENGTH-1)/BLOCK_LENGTH + 1 + 2)*BLOCK_LENGTH];
    unsigned char MAC[MAC_LENGTH];
    unsigned int * temp = (unsigned int *) (ENC_sender_ctx->nonce+4);
    //increase counter part of Nonce, the counter part is written small endian
    (ENC_sender_ctx->counter)++;
    *temp = ENC_sender_ctx->counter;

    formatData(TEMP, ENC_sender_ctx->nonce ,Payload, Plen);

    generateMAC(MAC, ENC_sender_ctx->key,TEMP,(Plen-1)/BLOCK_LENGTH + 1 +2);
    create_cyphered_counter_block(TEMP, ENC_sender_ctx->key, ENC_sender_ctx->nonce, (Plen-1)/BLOCK_LENGTH + 2);
    XOR(MAC, MAC, TEMP, MAC_LENGTH);
    XOR(ciphermessage->data, Payload, TEMP+BLOCK_LENGTH, Plen);


    ciphermessage->seq_number = ENC_sender_ctx->counter;
    ciphermessage->length = Plen + MAC_LENGTH;

    memcpy(ciphermessage->data + Plen,MAC,MAC_LENGTH);

 };

 /*
 * Decrypt the given ciphermessage, update receiver context and output
 * decryption in payload.
 *
 * payload              pointer to output array with size >= ciphermessage - MAC_LENGTH
 * payload_size         size of the output payload
 * ENC_receiver_ctx     pointer to the encoding receiver context
 * ciphermessage        pointer to the encrypted input message
 */
 void CCM_decrypt(unsigned char *payload, unsigned int *payload_size,  ENC_ctx *ENC_receiver_ctx, const message_ctx *ciphermessage){
    unsigned char MAC[MAC_LENGTH], MAC_check[MAC_LENGTH];
    unsigned char temp[ (1+((MAX_MESSAGE_LENGTH - MAC_LENGTH-1)/BLOCK_LENGTH)+2)*BLOCK_LENGTH];
    unsigned char temp_payload[MAX_MESSAGE_LENGTH - MAC_LENGTH];

    //check of message is not sended again:
    if (ENC_receiver_ctx->counter>=ciphermessage->seq_number){
        if (PRINT_errors == 1){
                printf("\n");
                printf("resending of message");
                printf("\n \n");};
        *payload_size = 0;
        return;
    };

    if (ciphermessage->length <= MAC_LENGTH){
        if (PRINT_errors == 1){
                printf("\n");
                printf("invalid1\n");
                printf("\n \n");};
        *payload_size = 0;
        return;
        };

    // set Nonce counter part to the received counter part (small endian)
    memcpy(ENC_receiver_ctx->nonce+4, &ciphermessage->seq_number, 4);

    create_cyphered_counter_block(temp, ENC_receiver_ctx->key, ENC_receiver_ctx->nonce, (ciphermessage->length -MAC_LENGTH-1)/BLOCK_LENGTH + 2);

    XOR(MAC, ciphermessage->data + ciphermessage->length-MAC_LENGTH,temp, MAC_LENGTH);
    XOR(temp_payload, ciphermessage->data, temp+BLOCK_LENGTH, ciphermessage->length - MAC_LENGTH);

    formatData(temp, ENC_receiver_ctx->nonce ,temp_payload, ciphermessage->length - MAC_LENGTH);

    generateMAC(MAC_check, ENC_receiver_ctx->key,temp,2+(ciphermessage->length - MAC_LENGTH-1)/BLOCK_LENGTH + 1);

    if (memcmp(MAC, MAC_check, MAC_LENGTH) != 0){
        if (PRINT_errors == 1){
                printf("\n");
                printf("invalid2\n");
                printf("\n \n");};
        *payload_size = 0;
        return;
    };
    ENC_receiver_ctx->counter =  ciphermessage->seq_number;
    *payload_size = ciphermessage->length - MAC_LENGTH;
    memcpy(payload,temp_payload, *payload_size);
 };

