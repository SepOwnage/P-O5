#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "wavpcm_io.h"
#include "decode.h"
#include "quantize.h"
#include "dequantize.h"
#include "bitmanipulation.h" 

struct parameters LowLowParams = { 8192, 19660, 10, 15 };
short LowLowValuesLeft[15];
struct start_values LowLowStartValuesLeft = { 0, 1, 0, 0, 0, LowLowValuesLeft };
struct parameters LowHighParams = { 16384, 1638, 10, 7 };
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

struct chunk historyChunk;
short quantizedBuffer[BUFFERSIZE * 3 / 4];
short wavbuffer[BUFFERSIZE];

/* This is the function that is called when the program starts. */
int main(int argc, char *argv[]){
	if (argc <= 1 || argv[1][0] == 'e' || 1) {//ENCODE    // or 1 to do in code (faster than changing launch settings)
		printf("Encoding\n");
		return mainencode();
	} else {//DECODE
		printf("Decoding\n");
		return maindecode();
	}
}
int mainencode() {
	struct wavpcm_input input;
	FILE *outputfile = fopen(COMPRESSEDFILE, "wb");
	FILE *notcompressedoutputfile = fopen("notcompressed.dat", "wb");
	int bufPos, bufIndex, read, quantPos;

	

	//init
	memset(&input, 0, sizeof(struct wavpcm_input));
	input.resource = INPUTWAVFILE;
	wavpcm_input_open(&input);
	memset(&historyChunk, 0, sizeof(struct chunk));

	for (bufPos = 0; bufPos < input.samplesAvailable; bufPos += (BUFFERSIZE / 2)) {
		
		//read a buffer
		read = wavpcm_input_read(&input, wavbuffer);
		if (read != BUFFERSIZE) {
			if ((input.samplesAvailable - bufPos) * 2 != read){
				printf("Warning: Not a full buffer read, amount read: %d. samplesAvailable: %d. bufPos: %d",
					read, input.samplesAvailable, bufPos );
			}
		}
			

		encode(wavbuffer, &historyChunk);

		//quantize the subbands
		//TODO: kill extra buffer?
		//left
		quantize(quantizedBuffer,
			historyChunk.leftLowEven, historyChunk.position2,
			BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
			5, &LowLowParams, &LowLowStartValuesLeft);
		quantize(quantizedBuffer + 5,
			historyChunk.rightLowEven, historyChunk.position2,
			BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
			5, &LowLowParams, &LowLowStartValuesRight);
		quantize(quantizedBuffer + 10,
			historyChunk.leftLowOdd, historyChunk.position2,
			BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
			5, &LowHighParams, &LowHighStartValuesLeft);
		quantize(quantizedBuffer + 15,
			historyChunk.rightLowOdd, historyChunk.position2,
			BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
			5, &LowHighParams, &LowHighStartValuesRight);
		quantize(quantizedBuffer + 20,
			historyChunk.leftHighEven, historyChunk.position2,
			BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
			5, &HighParams, &HighStartValuesLeft);
		quantize(quantizedBuffer + 25,
			historyChunk.rightHighEven, historyChunk.position2,
			BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
			5, &HighParams, &HighStartValuesRight);

		fwrite(quantizedBuffer, 2, 30, notcompressedoutputfile);
		compress30Samples(quantizedBuffer);
		fwrite(quantizedBuffer, 1, 15, outputfile);
	}

	fclose(outputfile);
	fclose(notcompressedoutputfile);
	wavpcm_input_close(&input);
	return 0;
}

int maindecode() {
	struct wavpcm_output output;
	FILE *inputfile = fopen(COMPRESSEDFILE, "rb");
	//FILE *notcompressedinputfile = fopen("notcompressed.dat", "rb");

	unsigned char readBuffer[15];
	unsigned char read = 0;
	unsigned char stop = 0;
	//init
	memset(&output, 0, sizeof(struct wavpcm_output));
	output.resource = OUTPUTWAVFILE;
	output.bitDepth = 16;
	output.channels = 1;
	output.samplingRate = 8000;
	wavpcm_output_open(&output);

	memset(&historyChunk, 0, sizeof(struct chunk));

	while (!stop) {
		read = 0;
		do {
			read += fread(readBuffer+read, 1, 15-read, inputfile);
		} while (!(read == 15 || feof(inputfile)));
		if (read != 15) {
			if (feof(inputfile)) {
				stop = 1;
				break;
			}
			printf("Error: End of file reached to soon: nb of bytes not multiple of 15");
			exit(1);
		}
		
		decompress30samples(readBuffer, quantizedBuffer);
		/*
		read = 0;
		do {
			read += fread(quantizedBuffer + read, 2, 30 - read, notcompressedinputfile);
		} while (!(read == 30 || feof(notcompressedinputfile)));
		if (read != 30) {
			if (feof(notcompressedinputfile)) {
				stop = 1;
				break;
			}
			printf("Error: End of file reached to soon: nb of bytes not multiple of 30");
			exit(1);
		}*/

		dequantize(quantizedBuffer, quantizedBuffer, 5, &LowLowParams, &LowLowStartValuesLeft);
		dequantize(quantizedBuffer + 5, quantizedBuffer + 5, 5, &LowLowParams, &LowLowStartValuesRight);
		dequantize(quantizedBuffer + 10, quantizedBuffer + 10, 5, &LowHighParams, &LowHighStartValuesLeft);
		dequantize(quantizedBuffer + 15, quantizedBuffer + 15, 5, &LowHighParams, &LowHighStartValuesRight);
		dequantize(quantizedBuffer + 20, quantizedBuffer + 20, 5, &HighParams, &HighStartValuesLeft);
		dequantize(quantizedBuffer + 25, quantizedBuffer + 25, 5, &HighParams, &HighStartValuesRight);

		decode(wavbuffer, 
			quantizedBuffer, quantizedBuffer + 10, quantizedBuffer + 20, 
			quantizedBuffer + 5, quantizedBuffer + 15, quantizedBuffer + 25, 
			&historyChunk);

		wavpcm_output_write(&output, wavbuffer, 40);
	}
	wavpcm_output_close(&output);
	fclose(inputfile);
	//fclose(notcompressedinputfile);
	return 0;
}

