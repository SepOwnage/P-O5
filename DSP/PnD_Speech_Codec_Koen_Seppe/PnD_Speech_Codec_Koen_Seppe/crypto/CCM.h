/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * CCM_encrypt.h
 *
 * Implementation of a CCM encryption and decryption function
 * that can be used in AES
 */

#ifndef __CCM_H
#define __CCM_H

void CCM_encrypt(message_ctx *ciphermessage, ENC_ctx *ENC_sender_ctx, const void *Payloadin, const unsigned int Plen);

void CCM_decrypt(void *payloadin, unsigned int *payload_size,  ENC_ctx *ENC_receiver_ctx, const message_ctx *ciphermessage);

#endif
