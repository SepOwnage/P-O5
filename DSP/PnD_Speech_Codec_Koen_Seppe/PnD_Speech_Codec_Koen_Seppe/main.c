#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "wavpcm_io.h"
#include "synthesis.h"
#include "quantizeStructs.h"
#include "quantizeTogether.h"
#include "dequantizeTogether.h"
#include "bitManipulation.h"

#ifdef CRYPTO
#include "crypto/global.h"
#include "crypto/util.h"
#include "crypto/STS_protocol.h"
#include "crypto/aes.h"
#include "crypto/CCM.h"
#include "crypto/cryptoMain.h"
#endif

#ifdef cheatMono
#include "quantizeMono.h"
#include "dequantizeMono.h"
#endif
#define NB_OF_SMALL_BUFFERS_IN_LARGE 40

//Quantize parameters
struct parameters LowLowParams = { 5144, 19660, 10, 15 };
struct parameters LowHighParams = { 16384, 328, 10, 7 };
struct parameters HighParams = { 29490, 31129, 10, 3 };

//quantize passed data (holding history between calls)
unsigned short LowLowValuesLeftQuantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
struct start_values LowLowStartValuesLeftQuantize = { 0, 1, 0, 0, 0, LowLowValuesLeftQuantize };
unsigned short LowLowValuesLeftDequantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
struct start_values LowLowStartValuesLeftDequantize = { 0, 1, 0, 0, 0, LowLowValuesLeftDequantize };
unsigned short LowHighValuesLeftQuantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned short LowHighValuesLeftDequantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
struct start_values LowHighStartValuesLeftQuantize = { 0, 1, 0, 0, 0, LowHighValuesLeftQuantize };
struct start_values LowHighStartValuesLeftDequantize = { 0, 1, 0, 0, 0, LowHighValuesLeftDequantize };
unsigned short HighValuesLeftQuantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned short HighValuesLeftDequantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
struct start_values HighStartValuesLeftQuantize = { 0, 1, 0, 0, 0, HighValuesLeftQuantize };
struct start_values HighStartValuesLeftDequantize = { 0, 1, 0, 0, 0, HighValuesLeftDequantize };
unsigned short LowLowValuesRightQuantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned short LowLowValuesRightDequantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
struct start_values LowLowStartValuesRightQuantize = { 0, 1, 0, 0, 0, LowLowValuesRightQuantize };
struct start_values LowLowStartValuesRightDequantize = { 0, 1, 0, 0, 0, LowLowValuesRightDequantize };
unsigned short LowHighValuesRightQuantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned short LowHighValuesRightDequantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
struct start_values LowHighStartValuesRightQuantize = { 0, 1, 0, 0, 0, LowHighValuesRightQuantize };
struct start_values LowHighStartValuesRightDequantize = { 0, 1, 0, 0, 0, LowHighValuesRightDequantize };
unsigned short HighValuesRightQuantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned short HighValuesRightDequantize[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
struct start_values HighStartValuesRightQuantize = { 0, 1, 0, 0, 0, HighValuesRightQuantize };
struct start_values HighStartValuesRightDequantize = { 0, 1, 0, 0, 0, HighValuesRightDequantize };

//chunks holding history for synthesis and analysis
struct chunk historyChunkAnalysis, historyChunkSynthesis;

//large buffer, passed to the crypto part
unsigned char largeCryptoBuffer[15 * NB_OF_SMALL_BUFFERS_IN_LARGE + 45];  //TODO reken die 30 juist uit
short placeInLargeBuffer=0;
//short buffer in which the input file is read
short wavbuffer[BUFFERSIZE];

#ifdef cheatMono
char stillMono = 1;
int i;
inline void quantizeBandsMono(short* start, struct chunk* theChunk){
  	quantizeMono(start,
		theChunk->leftLowEven, theChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
		5, &LowLowParams, &LowLowStartValuesLeftQuantize,start + 5,
		theChunk->rightLowEven, theChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
		&LowLowStartValuesRightQuantize);
  	quantizeMono(start + 10,
		theChunk->leftLowOdd, theChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
		5, &LowHighParams, &LowHighStartValuesLeftQuantize,start + 15,
		theChunk->rightLowOdd, theChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
		&LowHighStartValuesRightQuantize);
  	quantizeMono(start + 20,
		theChunk->leftHighEven, theChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
		5, &HighParams, &HighStartValuesLeftQuantize,start + 25,
		theChunk->rightHighEven, theChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
		&HighStartValuesRightQuantize);
}
inline void dequantizeBandsMono(short *buffer){
	dequantizeMono(buffer, buffer, 5, &LowLowParams, &LowLowStartValuesLeftDequantize,buffer + 5 , buffer + 5, &LowLowStartValuesRightDequantize);
	dequantizeMono(buffer + 10, buffer + 10, 5, &LowHighParams, &LowHighStartValuesLeftDequantize,buffer + 15, buffer + 15, &LowHighStartValuesRightDequantize);
	dequantizeMono(buffer + 20, buffer + 20, 5, &HighParams, &HighStartValuesLeftDequantize,buffer + 25, buffer + 25, &HighStartValuesRightDequantize);
}
#endif

//just a function to make the main function more compact. This calls quantize for the 3 used bands (one call is both left and right channel)
inline void quantizeBands(short* start, struct chunk* theChunk){
	 quantizeTogether(start,
			theChunk->leftLowEven, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			5, &LowLowParams, &LowLowStartValuesLeftQuantize,start + 5,
			theChunk->rightLowEven, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			&LowLowStartValuesRightQuantize);
	quantizeTogether(start + 10,
			theChunk->leftLowOdd, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			5, &LowHighParams, &LowHighStartValuesLeftQuantize,start + 15,
			theChunk->rightLowOdd, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			&LowHighStartValuesRightQuantize);
	quantizeTogether(start + 20,
			theChunk->leftHighEven, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			5, &HighParams, &HighStartValuesLeftQuantize,start + 25,
			theChunk->rightHighEven, theChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			&HighStartValuesRightQuantize);

}

//just a function to make the main function more compact. This calls dequantize for the 3 used bands (one call is both left and right channel)
inline void dequantizeBands(short *buffer){
	dequantizeTogether(buffer, buffer, 5, &LowLowParams, &LowLowStartValuesLeftDequantize,buffer + 5 , buffer + 5, &LowLowStartValuesRightDequantize);
	dequantizeTogether(buffer + 10, buffer + 10, 5, &LowHighParams, &LowHighStartValuesLeftDequantize,buffer + 15, buffer + 15, &LowHighStartValuesRightDequantize);
	dequantizeTogether(buffer + 20, buffer + 20, 5, &HighParams, &HighStartValuesLeftDequantize,buffer + 25, buffer + 25, &HighStartValuesRightDequantize);
}

struct wavpcm_input input;
struct wavpcm_output output;

#ifdef CRYPTO
unsigned int decrypt_size;
message_ctx ciphermessage;
RSA_ctx RSA_ctx_master, RSA_ctx_slave;
ENC_ctx ENC_ctx_master, ENC_ctx_slave;
#endif
/* This is the function that is called when the program starts. */
int main(int argc, char *argv[]){
	int bufPos, read;

	//initialization
	memset(&input, 0, sizeof(struct wavpcm_input));
	input.resource = INPUTWAVFILE;
	wavpcm_input_open(&input);
	memset(&output, 0, sizeof(struct wavpcm_output));
	output.resource = OUTPUTWAVFILE;
	wavpcm_output_copy_settings(&input, &output);
	wavpcm_output_open(&output);

	memset(&historyChunkAnalysis, 0, sizeof(struct chunk));
	memset(&historyChunkSynthesis, 0, sizeof(struct chunk));
	memset(largeCryptoBuffer, 0, sizeof(largeCryptoBuffer));
	memset(wavbuffer, 0, sizeof(wavbuffer));
	
#ifdef CRYPTO
	//Create RSA ctx master & slave
	calculate_parameters_RSA(&RSA_ctx_master);
	calculate_parameters_RSA(&RSA_ctx_slave);

	//Create ENC ctx master & slave
	ENC_ctx_master.counter = 0;
	ENC_ctx_slave.counter = 0;

	//Start protocol
	STSprotocol(&ENC_ctx_master, &ENC_ctx_slave, &RSA_ctx_master, &RSA_ctx_slave);
#endif
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
			}
			//process the buffer
			bufPos += read/2;

			analysis(wavbuffer, &historyChunkAnalysis); //Split bands into subbands
#ifdef cheatMono
			if(stillMono){ //Check if new buffer is still mono
				for(i=0; i < BUFFERSIZE_DIV2; i++){
					if(wavbuffer[2*i] != wavbuffer[2*i+1]){
						stillMono = 0;
						break;
					}
				}
			}
			if(stillMono){ //if still mono, use it to cheat (de)quantize
				quantizeBandsMono((short *) (largeCryptoBuffer + placeInLargeBuffer), &historyChunkAnalysis);
			}else{
#endif
			//quantize the bands, use largeCryptoBuffer to write the output in
			quantizeBands((short *) (largeCryptoBuffer + placeInLargeBuffer), &historyChunkAnalysis);
#ifdef cheatMono
			}
#endif
			//compress this new output (in place)
			compress30Samples((short *) (largeCryptoBuffer + placeInLargeBuffer)); //Compress samples to fit into the bytes that we give to crypto

			//increment position in largeCryptoBuffer
			placeInLargeBuffer += 15;
		}
#ifdef CRYPTO
		//encrypt
		sendData(&ENC_ctx_master, largeCryptoBuffer, sizeof(largeCryptoBuffer), &ciphermessage);
		//channel

		//decrypt
		readData(&ENC_ctx_slave, &ciphermessage, largeCryptoBuffer, &decrypt_size);
#endif
		//Now dequantize and merge bands back together
		placeInLargeBuffer = 0;
		while (placeInLargeBuffer < 15 * NB_OF_SMALL_BUFFERS_IN_LARGE) {
			//decompress. This is not in place because the amount of data expands. However, wavbuffer can be used again
			decompress30samples((largeCryptoBuffer + placeInLargeBuffer), wavbuffer);

			//dequantize the bands, working in place in wavbuffer
#ifdef cheatMono
			if(stillMono){ //if still mono, use it to cheat (de)quantize
				dequantizeBandsMono(wavbuffer);
			}else{
#endif
				dequantizeBands(wavbuffer);
#ifdef cheatMono
			}
#endif
			//synthesis filter bank, again using wavbuffer for both input and output
			synthesis(wavbuffer,
				wavbuffer, wavbuffer + 10, wavbuffer + 20, wavbuffer + 5, wavbuffer + 15, wavbuffer + 25,
				&historyChunkSynthesis);
			//write the output to file
			wavpcm_output_write(&output, wavbuffer, 40);
			placeInLargeBuffer += 15;
		}
	}
	wavpcm_input_close(&input);
	wavpcm_output_close(&output);
	return 0;

}
