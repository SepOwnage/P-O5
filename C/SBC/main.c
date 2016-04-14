#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "wavpcm_io.h"
#include "synthesis.h"
#include "quantize.h"
#include "dequantize.h"
#include "bitmanipulation.h" 

#define NB_OF_SMALL_BUFFERS_IN_LARGE 80

struct parameters LowLowParams = { 5144, 19660, 10, 15 };
short LowLowValuesLeft[15];
struct start_values LowLowStartValuesLeft = { 0, 1, 0, 0, 0, LowLowValuesLeft };
struct parameters LowHighParams = { 16384, 328, 10, 7 };
short LowHighValuesLeft[15];
struct start_values LowHighStartValuesLeft = { 0, 1, 0, 0, 0, LowHighValuesLeft };
struct parameters HighParams = { 29490, 31129, 10, 3 };
short HighValuesLeft[15];
struct start_values HighStartValuesLeft = { 0, 1, 0, 0, 0, HighValuesLeft };

short LowLowValuesRight[15];
struct start_values LowLowStartValuesRight = { 0, 1, 0, 0, 0, LowLowValuesRight };
short LowHighValuesRight[15];
struct start_values LowHighStartValuesRight = { 0, 1, 0, 0, 0, LowHighValuesRight };
short HighValuesRight[15];
struct start_values HighStartValuesRight = { 0, 1, 0, 0, 0, HighValuesRight };

struct chunk historyChunkAnalysis, historyChunkSynthesis;
unsigned char largeOutputBuffer[15 * NB_OF_SMALL_BUFFERS_IN_LARGE + 45];  //TODO reken die 30 juist uit
short placeInLargeBuffer=0;
short wavbuffer[BUFFERSIZE];



/* This is the function that is called when the program starts. */
int main(int argc, char *argv[]){
	struct wavpcm_input input, output;
	int bufPos, bufIndex, read, quantPos;
	unsigned char readBuffer[15];

	memset(&input, 0, sizeof(struct wavpcm_input));
	input.resource = INPUTWAVFILE;
	wavpcm_input_open(&input);

	memset(&output, 0, sizeof(struct wavpcm_output));
	output.resource = OUTPUTWAVFILE;
	wavpcm_output_copy_settings(&input, &output);
	wavpcm_output_open(&output);

	memset(&historyChunkAnalysis, 0, sizeof(struct chunk));
	memset(&historyChunkSynthesis, 0, sizeof(struct chunk));
	
	for (bufPos = 0; bufPos < input.samplesAvailable; bufPos += (BUFFERSIZE_DIV2)) {
		while (placeInLargeBuffer < 15* NB_OF_SMALL_BUFFERS_IN_LARGE) {
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

			quantize(largeOutputBuffer + placeInLargeBuffer,
				historyChunkAnalysis.leftLowEven, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &LowLowParams, &LowLowStartValuesLeft);
			quantize(largeOutputBuffer + placeInLargeBuffer + 5,
				historyChunkAnalysis.rightLowEven, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &LowLowParams, &LowLowStartValuesRight);
			quantize(largeOutputBuffer + placeInLargeBuffer + 10,
				historyChunkAnalysis.leftLowOdd, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &LowHighParams, &LowHighStartValuesLeft);
			quantize(largeOutputBuffer + placeInLargeBuffer + 15,
				historyChunkAnalysis.rightLowOdd, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &LowHighParams, &LowHighStartValuesRight);
			quantize(largeOutputBuffer + placeInLargeBuffer + 20,
				historyChunkAnalysis.leftHighEven, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &HighParams, &HighStartValuesLeft);
			quantize(largeOutputBuffer + placeInLargeBuffer + 25,
				historyChunkAnalysis.rightHighEven, historyChunkAnalysis.position2,
				BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
				5, &HighParams, &HighStartValuesRight);

			compress30Samples(largeOutputBuffer + placeInLargeBuffer);

			placeInLargeBuffer += 15;
		}

		//encrypt
		
		//channel

		//decrypt

		//undo speech part
		placeInLargeBuffer = 0;
		while (placeInLargeBuffer < 15* NB_OF_SMALL_BUFFERS_IN_LARGE) {

			decompress30samples(readBuffer, largeOutputBuffer + placeInLargeBuffer);
			
			//TODO put in one (inline?) function
			dequantize(largeOutputBuffer + placeInLargeBuffer, largeOutputBuffer + placeInLargeBuffer, 5, &LowLowParams, &LowLowStartValuesLeft);
			dequantize(largeOutputBuffer + placeInLargeBuffer + 5, largeOutputBuffer + placeInLargeBuffer + 5, 5, &LowLowParams, &LowLowStartValuesRight);
			dequantize(largeOutputBuffer + placeInLargeBuffer + 10, largeOutputBuffer + placeInLargeBuffer + 10, 5, &LowHighParams, &LowHighStartValuesLeft);
			dequantize(largeOutputBuffer + placeInLargeBuffer + 15, largeOutputBuffer + placeInLargeBuffer + 15, 5, &LowHighParams, &LowHighStartValuesRight);
			dequantize(largeOutputBuffer + placeInLargeBuffer + 20, largeOutputBuffer + placeInLargeBuffer + 20, 5, &HighParams, &HighStartValuesLeft);
			dequantize(largeOutputBuffer + placeInLargeBuffer + 25, largeOutputBuffer + placeInLargeBuffer + 25, 5, &HighParams, &HighStartValuesRight);

			synthesis(wavbuffer,
				largeOutputBuffer + placeInLargeBuffer, largeOutputBuffer + placeInLargeBuffer + 10, largeOutputBuffer + placeInLargeBuffer + 20,
				largeOutputBuffer + placeInLargeBuffer + 5, largeOutputBuffer + placeInLargeBuffer + 15, largeOutputBuffer + placeInLargeBuffer + 25,
				&historyChunkSynthesis);

			wavpcm_output_write(&output, wavbuffer, 40);
		}

}