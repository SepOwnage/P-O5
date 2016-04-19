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

#define NB_OF_SMALL_BUFFERS_IN_LARGE 80

struct parameters LowLowParams = { 5144, 19660, 10, 15 };
unsigned short LowLowValuesLeft[15];
struct start_values LowLowStartValuesLeft = { 0, 1, 0, 0, 0, LowLowValuesLeft };
struct parameters LowHighParams = { 16384, 328, 10, 7 };
unsigned short LowHighValuesLeft[15];
struct start_values LowHighStartValuesLeft = { 0, 1, 0, 0, 0, LowHighValuesLeft };
struct parameters HighParams = { 29490, 31129, 10, 3 };
unsigned short HighValuesLeft[15];
struct start_values HighStartValuesLeft = { 0, 1, 0, 0, 0, HighValuesLeft };

unsigned short LowLowValuesRight[15];
struct start_values LowLowStartValuesRight = { 0, 1, 0, 0, 0, LowLowValuesRight };
unsigned short LowHighValuesRight[15];
struct start_values LowHighStartValuesRight = { 0, 1, 0, 0, 0, LowHighValuesRight };
unsigned short HighValuesRight[15];
struct start_values HighStartValuesRight = { 0, 1, 0, 0, 0, HighValuesRight };

struct chunk historyChunkAnalysis, historyChunkSynthesis;
unsigned char largeCryptoBuffer[15 * NB_OF_SMALL_BUFFERS_IN_LARGE + 45];  //TODO reken die 30 juist uit
short placeInLargeBuffer=0;
short wavbuffer[BUFFERSIZE];



/* This is the function that is called when the program starts. */
int main(int argc, char *argv[]){
	struct wavpcm_input input;
	struct wavpcm_output output;
	int bufPos, bufIndex, read, quantPos;
	unsigned char readBuffer[15];
	unsigned int decrypt_size;
	message_ctx ciphermessage;
	RSA_ctx RSA_ctx_master, RSA_ctx_slave;
	ENC_ctx ENC_ctx_master, ENC_ctx_slave;

	memset(&input, 0, sizeof(struct wavpcm_input));
	input.resource = INPUTWAVFILE;
	wavpcm_input_open(&input);

	memset(&output, 0, sizeof(struct wavpcm_output));
	output.resource = OUTPUTWAVFILE;
	wavpcm_output_copy_settings(&input, &output);
	wavpcm_output_open(&output);

	memset(&historyChunkAnalysis, 0, sizeof(struct chunk));
	memset(&historyChunkSynthesis, 0, sizeof(struct chunk));
	
	//Create RSA ctx master & slave
	calculate_parameters_RSA(&RSA_ctx_master);
	calculate_parameters_RSA(&RSA_ctx_slave);

	//Create ENC ctx master & slave
	ENC_ctx_master.counter = 0;
	ENC_ctx_slave.counter = 0;

	//Start protocol
	STSprotocol(&ENC_ctx_master, &ENC_ctx_slave, &RSA_ctx_master, &RSA_ctx_slave);

	for (bufPos = 0; bufPos < input.samplesAvailable; bufPos += (BUFFERSIZE_DIV2)) {
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

			analysis(wavbuffer, &historyChunkAnalysis);
			//TODO: in (inlined) subfunction

			quantize((short *) (largeCryptoBuffer + placeInLargeBuffer),
				historyChunkAnalysis.leftLowEven, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &LowLowParams, &LowLowStartValuesLeft);
			quantize((short *)(largeCryptoBuffer + placeInLargeBuffer + 5),
				historyChunkAnalysis.rightLowEven, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &LowLowParams, &LowLowStartValuesRight);
			quantize((short *)(largeCryptoBuffer + placeInLargeBuffer + 10),
				historyChunkAnalysis.leftLowOdd, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &LowHighParams, &LowHighStartValuesLeft);
			quantize((short *)(largeCryptoBuffer + placeInLargeBuffer + 15),
				historyChunkAnalysis.rightLowOdd, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &LowHighParams, &LowHighStartValuesRight);
			quantize((short *)(largeCryptoBuffer + placeInLargeBuffer + 20),
				historyChunkAnalysis.leftHighEven, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &HighParams, &HighStartValuesLeft);
			quantize((short *)(largeCryptoBuffer + placeInLargeBuffer + 25),
				historyChunkAnalysis.rightHighEven, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &HighParams, &HighStartValuesRight);

			compress30Samples((short *) (largeCryptoBuffer + placeInLargeBuffer));

			placeInLargeBuffer += 15;
		}

		//encrypt
		sendData(&ENC_ctx_master, largeCryptoBuffer, sizeof(largeCryptoBuffer), &ciphermessage);
		//channel

		//decrypt
		readData(&ENC_ctx_slave, &ciphermessage, largeCryptoBuffer, &decrypt_size);

		//undo speech part
		placeInLargeBuffer = 0;
		while (placeInLargeBuffer < 15 * NB_OF_SMALL_BUFFERS_IN_LARGE) {

			decompress30samples((largeCryptoBuffer + placeInLargeBuffer), wavbuffer);

			//TODO put in one (inline?) function
			dequantize(wavbuffer, wavbuffer, 5, &LowLowParams, &LowLowStartValuesLeft);
			dequantize(wavbuffer + 5 , wavbuffer + 5, 5, &LowLowParams, &LowLowStartValuesRight);
			dequantize(wavbuffer + 10, wavbuffer + 10, 5, &LowHighParams, &LowHighStartValuesLeft);
			dequantize(wavbuffer + 15, wavbuffer + 15, 5, &LowHighParams, &LowHighStartValuesRight);
			dequantize(wavbuffer + 20, wavbuffer + 20, 5, &HighParams, &HighStartValuesLeft);
			dequantize(wavbuffer + 25, wavbuffer + 25, 5, &HighParams, &HighStartValuesRight);

			synthesis(wavbuffer,
				wavbuffer, wavbuffer + 10, wavbuffer + 20, wavbuffer + 5, wavbuffer + 15, wavbuffer + 25,
				&historyChunkSynthesis);

			wavpcm_output_write(&output, wavbuffer, 40);
			placeInLargeBuffer += 15;
		}
	}
}