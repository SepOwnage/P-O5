#include <time.h>
#include "global.h"
#include "util.h"
#include "STS_protocol.h"
#include "aes.h"
#include "CCM.h"

void STSprotocol(ENC_ctx *ENC_ctx_master, ENC_ctx *ENC_ctx_slave,
	const RSA_ctx *RSA_ctx_master, const RSA_ctx *RSA_ctx_slave);
void sendData(ENC_ctx *ENC_ctx_sender, const unsigned char *input, const unsigned int input_size, message_ctx *ciphermessage);
void readData(ENC_ctx *ENC_ctx_receiver, const message_ctx *ciphermessage, unsigned char *out, unsigned int *output_size);
