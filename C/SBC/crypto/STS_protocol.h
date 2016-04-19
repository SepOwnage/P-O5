/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * STS_protocol.h
 *
 * Implementation of the STS-protocol messages
 */

#ifndef __STS_PROTOCOL_H
#define __STS_PROTOCOL_H

void calculate_parameters_RSA(RSA_ctx *ctx);

void STS_message1_master(message_ctx *message, STS_ctx *STS_ctx_master);
void STS_message1_slave(message_ctx *message, STS_ctx *STS_ctx_slave, const RSA_ctx *RSA_ctx_slave, const message_ctx *input);
void STS_message2_master(message_ctx *message, ENC_ctx *ENC_ctx_master, STS_ctx *STS_ctx_master, const RSA_ctx *RSA_ctx_master, const message_ctx *input);
void STS_message2_slave(message_ctx *message, ENC_ctx *ENC_ctx_slave, STS_ctx *STS_ctx_slave, const RSA_ctx *RSA_ctx_slave, const message_ctx *input);
void STS_message3_master(message_ctx *message, unsigned char *status, ENC_ctx *ENC_ctx_master, STS_ctx *STS_ctx_master, const RSA_ctx *RSA_ctx_master, const message_ctx *input);
void STS_message3_slave(message_ctx *message, ENC_ctx *ENC_ctx_slave, STS_ctx *STS_ctx_slave, const RSA_ctx *RSA_ctx_slave, const message_ctx *input);

#endif
