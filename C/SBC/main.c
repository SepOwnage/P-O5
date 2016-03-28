#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "wavpcm_io.h"
#include "encode.h"

/*
static short mus[4] = MUS;
static short phis[4] = PHIS;
static short buffer_lengths[4] = BUFFER_LENGTHS;
*/

struct wavpcm_input input;
struct wavpcm_output output_LL, output_LH, output_HL, output_HH, output_L, output_H;
//the buffer in which input is read
short buffer[BUFFERSIZE];
//for testing purposes, we'll write every subband to a file => output buffer for each subband
short reconstructedBuffer_LL[BUFFERSIZE / 4];
short reconstructedBuffer_LH[BUFFERSIZE / 4];
short reconstructedBuffer_HL[BUFFERSIZE / 4];
short reconstructedBuffer_HH[BUFFERSIZE / 4];
short reconstructedBuffer_L[BUFFERSIZE / 2];
short reconstructedBuffer_H[BUFFERSIZE / 2];

//The chunk to pass between calls of encode  (contains previous samples)
struct chunk theChunk;
//Bookkeeping
int bufPos, bufIndex, read;

/* This is the function that is called when the program starts. */
int main(int argc, char *argv[])
{
	//Variable initialization (to 0)

	memset(&theChunk, 0, sizeof(struct chunk));

	memset(&input, 0, sizeof(struct wavpcm_input)); //Fill sizeof(...) bytes starting from input with
	input.resource = "E:\\Koen\\Documenten\\2015-2016\\pno\\C\\SBC\\input.wav";
	memset(&output_LL, 0, sizeof(struct wavpcm_output));
	memset(&output_LH, 0, sizeof(struct wavpcm_output));
	memset(&output_HL, 0, sizeof(struct wavpcm_output));
	memset(&output_HH, 0, sizeof(struct wavpcm_output));
	memset(&output_L, 0, sizeof(struct wavpcm_output));
	memset(&output_H, 0, sizeof(struct wavpcm_output));
	output_LL.resource = "output_LL.wav";
	output_LH.resource = "output_LH.wav";
	output_HL.resource = "output_HL.wav";
	output_HH.resource = "output_HH.wav";
	output_L.resource = "output_L.wav";
	output_H.resource = "output_H.wav";

	/* First open input file and parse header, */
	wavpcm_input_open(&input);
	/* and then copy this same header configuration for the output file */
	wavpcm_output_copy_settings(&input, &output_LL);
	(output_LL.samplingRate) /= 4;
	wavpcm_output_copy_settings(&input, &output_LH);
	(output_LH.samplingRate) /= 4;
	wavpcm_output_copy_settings(&input, &output_HL);
	(output_HL.samplingRate) /= 4;
	wavpcm_output_copy_settings(&input, &output_HH);
	(output_HH.samplingRate) /= 4;
	wavpcm_output_copy_settings(&input, &output_L);
	(output_HH.samplingRate) /= 2;
	wavpcm_output_copy_settings(&input, &output_H);
	(output_HH.samplingRate) /= 2;
	wavpcm_output_open(&output_LL);
	wavpcm_output_open(&output_LH);
	wavpcm_output_open(&output_HL);
	wavpcm_output_open(&output_HH);
	wavpcm_output_open(&output_L);
	wavpcm_output_open(&output_H);

	/*bufPos expressed in temporal samples*/
	for (bufPos = 0; bufPos < input.samplesAvailable; bufPos += (BUFFERSIZE / 2)) {
		/* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
		/* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
		read = wavpcm_input_read(&input, buffer);
		if (read != BUFFERSIZE)
			printf("Not a full buffer read, amount read: %d", read);

		/* transform buffer */
		encode(buffer, &theChunk);

		//fill all output buffers...
		for (bufIndex = 0; bufIndex < BUFFERSIZE / 8; bufIndex++) {
			reconstructedBuffer_LL[2 * bufIndex] = theChunk.leftLowEven[(bufIndex + theChunk.position2) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
			reconstructedBuffer_LL[2 * bufIndex + 1] = theChunk.rightLowEven[(bufIndex + theChunk.position2) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
			reconstructedBuffer_LH[2 * bufIndex] = theChunk.leftLowOdd[(bufIndex + theChunk.position2) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
			reconstructedBuffer_LH[2 * bufIndex + 1] = theChunk.rightLowOdd[(bufIndex + theChunk.position2) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
			reconstructedBuffer_HL[2 * bufIndex] = theChunk.leftHighEven[(bufIndex + theChunk.position2) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
			reconstructedBuffer_HL[2 * bufIndex + 1] = theChunk.rightHighEven[(bufIndex + theChunk.position2) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
			reconstructedBuffer_HH[2 * bufIndex] = theChunk.leftHighOdd[(bufIndex + theChunk.position2) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
			reconstructedBuffer_HH[2 * bufIndex + 1] = theChunk.rightHighOdd[(bufIndex + theChunk.position2) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
		}
		for (bufIndex = 0; bufIndex < BUFFERSIZE / 4; bufIndex++) {
			reconstructedBuffer_L[2 * bufIndex] = theChunk.leftEven[(bufIndex + theChunk.position1) % (BUFFERSIZE / 4 + LENGTH_FILTER1_HALF)];
			reconstructedBuffer_L[2 * bufIndex + 1] = theChunk.rightEven[(bufIndex + theChunk.position1) % (BUFFERSIZE / 4 + LENGTH_FILTER1_HALF)];
			reconstructedBuffer_H[2 * bufIndex] = theChunk.leftOdd[(bufIndex + theChunk.position1) % (BUFFERSIZE / 4 + LENGTH_FILTER1_HALF)];
			reconstructedBuffer_H[2 * bufIndex + 1] = theChunk.rightOdd[(bufIndex + theChunk.position1) % (BUFFERSIZE / 4 + LENGTH_FILTER1_HALF)];
		}
		//... and dump them to their files
		wavpcm_output_write(&output_LL, reconstructedBuffer_LL, read / 4);
		wavpcm_output_write(&output_LH, reconstructedBuffer_LH, read / 4);
		wavpcm_output_write(&output_HL, reconstructedBuffer_HL, read / 4);
		wavpcm_output_write(&output_HH, reconstructedBuffer_HH, read / 4);
		wavpcm_output_write(&output_L, reconstructedBuffer_L, read / 2);
		wavpcm_output_write(&output_H, reconstructedBuffer_H, read / 2);
	}

	/* finalize output (write header) and close */
	wavpcm_output_close(&output_LL);
	wavpcm_output_close(&output_LH);
	wavpcm_output_close(&output_HL);
	wavpcm_output_close(&output_HH);
	wavpcm_output_close(&output_L);
	wavpcm_output_close(&output_H);
	wavpcm_input_close(&input);
	/* Return successful exit code. */
	return 0;
}
