#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "wavpcm_io.h"
#include "decode.h"


struct wavpcm_input input_LL, input_LH, input_HL;
struct wavpcm_output output;
short buffer[BUFFERSIZE];
short LLbuffer[BUFFERSIZE];
short LHbuffer[BUFFERSIZE];
short HLbuffer[BUFFERSIZE];

struct chunk theChunk;


/* This is the function that is called when the program starts. */
int main(int argc, char *argv[])
{
	/* Variable declarations. */
	int bufPos, bufIndex, read;
	int i;

	memset(&theChunk, 0, sizeof(struct chunk));

	memset(&output, 0, sizeof(struct wavpcm_output)); //Fill sizeof(...) bytes starting from input with
	output.resource = "output.wav";
	memset(&input_LL, 0, sizeof(struct wavpcm_input));
	memset(&input_LH, 0, sizeof(struct wavpcm_input));
	memset(&input_HL, 0, sizeof(struct wavpcm_input));
	input_LL.resource = "../saved_subbands/LL.wav";
	input_LH.resource = "../saved_subbands/LH.wav";
	input_HL.resource = "../saved_subbands/HL.wav";

	/* First open input file and parse header, */
	wavpcm_input_open(&input_LL);
	wavpcm_input_open(&input_LH);
	wavpcm_input_open(&input_HL);
	/* and then use this same header configuration for the output file */
	wavpcm_output_copy_settings(&input_LL, &output);
	(output.samplingRate) *= 4;
	wavpcm_output_open(&output);

	/*bufPos expressed in temporal samples*/
	for (bufPos = 0; bufPos < input_LL.samplesAvailable; bufPos += (BUFFERSIZE / 2)) {
		/* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
		/* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
		read = wavpcm_input_read(&input_LL, LLbuffer);
		if (read != BUFFERSIZE)
			printf("Not a full buffer read, amount read: %d", read);
		read = wavpcm_input_read(&input_LH, LHbuffer);
		if (read != BUFFERSIZE)
			printf("Not a full buffer read, amount read: %d", read);
		read = wavpcm_input_read(&input_HL, HLbuffer);
		if (read != BUFFERSIZE)
			printf("Not a full buffer read, amount read: %d", read);

		//make mono
		for (int i = 0; i < BUFFERSIZE / 2; i++) {
			LLbuffer[i] = LLbuffer[2 * i];
			LHbuffer[i] = LHbuffer[2 * i];
			HLbuffer[i] = HLbuffer[2 * i];
		}

		//run four times (read BUFFERSIZE instead of BUFFERSIZE/4. Not an argument of wavpcm_input_read :(  )
		for (int i = 0; i < 20; i += 5) {
			decode(buffer, LLbuffer + i, LHbuffer + i, HLbuffer + i, LLbuffer + i, LHbuffer + i, HLbuffer + i, &theChunk);
			wavpcm_output_write(&output, buffer, BUFFERSIZE);
		}
	}

	/* finalize output (write header) and close */
	wavpcm_input_close(&input_LL);
	wavpcm_input_close(&input_HL);
	wavpcm_input_close(&input_LH);
	wavpcm_output_close(&output);
	/* Return successful exit code. */
	return 0;
}