#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "wavpcm_io.h"
#include "synthesis.h"
#include "quantize.h"
#include "dequantize.h"
#include "bitmanipulation.h" 
#include <time.h>
#include "crypto/global.h"
#include "crypto/util.h"
#include "crypto/STS_protocol.h"
#include "crypto/aes.h"
#include "crypto/CCM.h"
#include "crypto/cryptoMain.h"

#define NB_OF_SMALL_BUFFERS_IN_LARGE 40

struct parameters LowLowParams = { 5144, 19660, 10, 15 };
unsigned short LowLowValuesLeftQuantize[15];
struct start_values LowLowStartValuesLeftQuantize = { 0, 1, 0, 0, 0, LowLowValuesLeftQuantize };
unsigned short LowLowValuesLeftDequantize[15];
struct start_values LowLowStartValuesLeftDequantize = { 0, 1, 0, 0, 0, LowLowValuesLeftDequantize };
struct parameters LowHighParams = { 16384, 328, 10, 7 };
unsigned short LowHighValuesLeftQuantize[15];
unsigned short LowHighValuesLeftDequantize[15];
struct start_values LowHighStartValuesLeftQuantize = { 0, 1, 0, 0, 0, LowHighValuesLeftQuantize };
struct start_values LowHighStartValuesLeftDequantize = { 0, 1, 0, 0, 0, LowHighValuesLeftDequantize };

struct parameters HighParams = { 29490, 31129, 10, 3 };
unsigned short HighValuesLeftQuantize[15];
unsigned short HighValuesLeftDequantize[15];
struct start_values HighStartValuesLeftQuantize = { 0, 1, 0, 0, 0, HighValuesLeftQuantize };
struct start_values HighStartValuesLeftDequantize = { 0, 1, 0, 0, 0, HighValuesLeftDequantize };

unsigned short LowLowValuesRightQuantize[15];
unsigned short LowLowValuesRightDequantize[15];
struct start_values LowLowStartValuesRightQuantize = { 0, 1, 0, 0, 0, LowLowValuesRightQuantize };
struct start_values LowLowStartValuesRightDequantize = { 0, 1, 0, 0, 0, LowLowValuesRightDequantize };
unsigned short LowHighValuesRightQuantize[15];
unsigned short LowHighValuesRightDequantize[15];
struct start_values LowHighStartValuesRightQuantize = { 0, 1, 0, 0, 0, LowHighValuesRightQuantize };
struct start_values LowHighStartValuesRightDequantize = { 0, 1, 0, 0, 0, LowHighValuesRightDequantize };
unsigned short HighValuesRightQuantize[15];
unsigned short HighValuesRightDequantize[15];
struct start_values HighStartValuesRightQuantize = { 0, 1, 0, 0, 0, HighValuesRightQuantize };
struct start_values HighStartValuesRightDequantize = { 0, 1, 0, 0, 0, HighValuesRightDequantize };

struct chunk historyChunkAnalysis, historyChunkSynthesis;
unsigned char largeCryptoBuffer[15 * NB_OF_SMALL_BUFFERS_IN_LARGE + 45];  //TODO reken die 30 juist uit
short placeInLargeBuffer=0;
short wavbuffer[BUFFERSIZE];



inline void quantizeBands(short* start, struct chunk* theChunk){
	quantize(start,
			theChunk->leftLowEven, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			5, &LowLowParams, &LowLowStartValuesLeftQuantize);
	quantize(start + 5,
			theChunk->rightLowEven, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			5, &LowLowParams, &LowLowStartValuesRightQuantize);
	quantize(start + 10,
			theChunk->leftLowOdd, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			5, &LowHighParams, &LowHighStartValuesLeftQuantize);
	quantize(start + 15,
			theChunk->rightLowOdd, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			5, &LowHighParams, &LowHighStartValuesRightQuantize);
	quantize(start + 20,
			theChunk->leftHighEven, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			5, &HighParams, &HighStartValuesLeftQuantize);
	quantize(start + 25,
			theChunk->rightHighEven, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			5, &HighParams, &HighStartValuesRightQuantize);
}
inline void dequantizeBands(short *buffer){
	dequantize(buffer, buffer, 5, &LowLowParams, &LowLowStartValuesLeftDequantize);
	dequantize(buffer + 5 , buffer + 5, 5, &LowLowParams, &LowLowStartValuesRightDequantize);
	dequantize(buffer + 10, buffer + 10, 5, &LowHighParams, &LowHighStartValuesLeftDequantize);
	dequantize(buffer + 15, buffer + 15, 5, &LowHighParams, &LowHighStartValuesRightDequantize);
	dequantize(buffer + 20, buffer + 20, 5, &HighParams, &HighStartValuesLeftDequantize);
	dequantize(buffer + 25, buffer + 25, 5, &HighParams, &HighStartValuesRightDequantize);
}

struct wavpcm_input input;
struct wavpcm_output output;
/*
unsigned int decrypt_size;
message_ctx ciphermessage;
RSA_ctx RSA_ctx_master, RSA_ctx_slave;
ENC_ctx ENC_ctx_master, ENC_ctx_slave;*/

/* This is the function that is called when the program starts. */
int main(int argc, char *argv[]){
	int bufPos, read;

	input.resource = INPUTWAVFILE;
	wavpcm_input_open(&input);

	memset(&output, 0, sizeof(struct wavpcm_output));
	output.resource = OUTPUTWAVFILE;
	wavpcm_output_copy_settings(&input, &output);
	wavpcm_output_open(&output);

	memset(&historyChunkAnalysis, 0, sizeof(struct chunk));
	memset(&historyChunkSynthesis, 0, sizeof(struct chunk));
	
	//Create RSA ctx master & slave
	//calculate_parameters_RSA(&RSA_ctx_master);
	//calculate_parameters_RSA(&RSA_ctx_slave);

	//Create ENC ctx master & slave
	//ENC_ctx_master.counter = 0;
	//ENC_ctx_slave.counter = 0;

	//Start protocol
	//STSprotocol(&ENC_ctx_master, &ENC_ctx_slave, &RSA_ctx_master, &RSA_ctx_slave);

	for (bufPos = 0; bufPos < input.samplesAvailable; ) {
		placeInLargeBuffer = 0;
		while (placeInLargeBuffer < 15 * NB_OF_SMALL_BUFFERS_IN_LARGE) {
			//read a buffer
			read = wavpcm_input_read(&input, wavbuffer);
			if (read != BUFFERSIZE) {
				if ((input.samplesAvailable - bufPos) * 2 != read) {
					printf("Warning: Not a full buffer read, amount read: %d. samplesAvailable: %d. bufPos: %d",
						read, input.samplesAvailable, bufPos);
				}
				//TODO: fill with zeros? ignore?
			}
			bufPos += read/2;

			analysis(wavbuffer, &historyChunkAnalysis);
			quantizeBands((short *) (largeCryptoBuffer + placeInLargeBuffer), &historyChunkAnalysis);

			compress30Samples((short *) (largeCryptoBuffer + placeInLargeBuffer));

			placeInLargeBuffer += 15;
		}

		//encrypt
		//sendData(&ENC_ctx_master, largeCryptoBuffer, sizeof(largeCryptoBuffer), &ciphermessage);
		//channel

		//decrypt
		//readData(&ENC_ctx_slave, &ciphermessage, largeCryptoBuffer, &decrypt_size);

		//undo speech part
		placeInLargeBuffer = 0;
		while (placeInLargeBuffer < 15 * NB_OF_SMALL_BUFFERS_IN_LARGE) {

			decompress30samples((largeCryptoBuffer + placeInLargeBuffer), wavbuffer);

			dequantizeBands(wavbuffer);

			synthesis(wavbuffer,
				wavbuffer, wavbuffer + 10, wavbuffer + 20, wavbuffer + 5, wavbuffer + 15, wavbuffer + 25,
				&historyChunkSynthesis);

			wavpcm_output_write(&output, wavbuffer, 40);
			placeInLargeBuffer += 15;
		}
	}
	wavpcm_input_close(&input);
	wavpcm_output_close(&output);
	return 0;

}
