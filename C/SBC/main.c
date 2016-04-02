#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "wavpcm_io.h"
#include "encode.h"
#include "quantize.h"
#include "dequantize.h"

/*
static short mus[4] = MUS;
static short phis[4] = PHIS;
static short buffer_lengths[4] = BUFFER_LENGTHS;
*/

struct wavpcm_input input;
struct wavpcm_output output;
short buffer[BUFFERSIZE];
short left_buffer[BUFFERSIZE / 2];
short quantizedbuffer[5];
short outputbuffer[5];
short ToWavBuffer[10];
struct parameters params = {8192,19660,10,15};
unsigned short valuesbufferEncode[10] = {0,0,0,0,0,0,0,0,0,0};
unsigned short valuesbufferDecode[10] = {0,0,0,0,0,0,0,0,0,0};
struct start_values valuesEncode = {0,1,0,0,0,valuesbufferEncode};
struct start_values valuesDecode = {0,1,0,0,0,valuesbufferDecode};



/* This is the function that is called when the program starts. */
int main (int argc, char *argv[])
{
  /* Variable declarations. */
  int bufPos, bufIndex, read, quantPos;
  int i;


  memset(&input, 0, sizeof(struct wavpcm_input)); //Fill sizeof(...) bytes starting from input with
  input.resource=INPUTWAVFILE;
  memset(&output, 0, sizeof(struct wavpcm_output));
  output.resource = OUTPUTWAVFILE;
 /* First open input file and parse header, */
  wavpcm_input_open (&input);
  wavpcm_output_copy_settings(&input, &output);
  wavpcm_output_open(&output);
  for (bufPos=0; bufPos<input.samplesAvailable; bufPos+=(BUFFERSIZE/2)) {
  /*bufPos expressed in temporal samples*/
    /* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
    /* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
    read = wavpcm_input_read (&input, buffer);
	for (i = 0; i < 80 / 2; i++)
		left_buffer[i] = buffer[2 * i];

    /* transform buffer */
	for (quantPos = 0; quantPos<4; quantPos++){
		quantize(quantizedbuffer, left_buffer, quantPos * 5, BUFFERSIZE / 2, 5, &params, &valuesEncode);
        dequantize(outputbuffer, quantizedbuffer, 5, &params, &valuesDecode);
		for (i = 0; i < 5; i++){
			ToWavBuffer[2 * i] = ToWavBuffer[2 * i + 1] = outputbuffer[i];
			printf("%d, ", ToWavBuffer[2*i]);
			printf("%d, ", ToWavBuffer[2*i+1]);
		}
		wavpcm_output_write(&output, ToWavBuffer, 10);
	}
	printf("\n");
  }

  wavpcm_output_close(&output);
  wavpcm_input_close(&input);
  /* Return successful exit code. */
  return 0;
}
