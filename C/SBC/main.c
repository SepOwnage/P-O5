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
struct wavpcm_output output_LL, output_LH, output_HL, output_HH;
short buffer[BUFFERSIZE];
short reconstructedBuffer_LL[BUFFERSIZE / 4];
short reconstructedBuffer_LH[BUFFERSIZE / 4];
short reconstructedBuffer_HL[BUFFERSIZE / 4];
short reconstructedBuffer_HH[BUFFERSIZE / 4];

struct chunk theChunk;
int bufPos, bufIndex, read;

/* This is the function that is called when the program starts. */
int main(int argc, char *argv[])
{
	/* Variable declarations. */

	memset(&theChunk, 0, sizeof(struct chunk));

	memset(&input, 0, sizeof(struct wavpcm_input)); //Fill sizeof(...) bytes starting from input with
	input.resource = "E:\\Koen\\Documenten\\2015-2016\\pno\\C\\SBC\\input.wav";
	memset(&output_LL, 0, sizeof(struct wavpcm_output));
	memset(&output_LH, 0, sizeof(struct wavpcm_output));
	memset(&output_HL, 0, sizeof(struct wavpcm_output));
	memset(&output_HH, 0, sizeof(struct wavpcm_output));
	output_LL.resource = "output_LL.wav";
	output_LH.resource = "output_LH.wav";
	output_HL.resource = "output_HL.wav";
	output_HH.resource = "output_HH.wav";


	/* First open input file and parse header, */
	wavpcm_input_open(&input);
	/* and then use this same header configuration for the output file */
	wavpcm_output_copy_settings(&input, &output_LL);
	(output_LL.samplingRate) /= 4;
	wavpcm_output_copy_settings(&input, &output_LH);
	(output_LH.samplingRate) /= 4;
	wavpcm_output_copy_settings(&input, &output_HL);
	(output_HL.samplingRate) /= 4;
	wavpcm_output_copy_settings(&input, &output_HH);
	(output_HH.samplingRate) /= 4;
	wavpcm_output_open(&output_LL);
	wavpcm_output_open(&output_LH);
	wavpcm_output_open(&output_HL);
	wavpcm_output_open(&output_HH);

	/*bufPos expressed in temporal samples*/
	for (bufPos = 0; bufPos < input.samplesAvailable; bufPos += (BUFFERSIZE / 2)) {
		/* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
		/* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
		read = wavpcm_input_read(&input, buffer);
		if (read != BUFFERSIZE)
			printf("Not a full buffer read, amount read: %d", read);

		/* transform buffer */
		encode(buffer, &theChunk);

		/* if required, dump compressed output */

		/* inverse transform buffer */
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
		/* dump reconstructed output */
		wavpcm_output_write(&output_LL, reconstructedBuffer_LL, read / 4);
		wavpcm_output_write(&output_LH, reconstructedBuffer_LH, read / 4);
		wavpcm_output_write(&output_HL, reconstructedBuffer_HL, read / 4);
		wavpcm_output_write(&output_HH, reconstructedBuffer_HH, read / 4);
	}

	/* finalize output (write header) and close */
	wavpcm_output_close(&output_LL);
	wavpcm_output_close(&output_LH);
	wavpcm_output_close(&output_HL);
	wavpcm_output_close(&output_HH);

	/* Return successful exit code. */
	return 0;
}
