#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "wavpcm_io.h"
#include "encode.h"
#include "quantize.h"

/*
static short mus[4] = MUS;
static short phis[4] = PHIS;
static short buffer_lengths[4] = BUFFER_LENGTHS;
*/

struct wavpcm_input input;
//struct wavpcm_output output_LL, output_LH, output_HL, output_HH;
short buffer[BUFFERSIZE];
short dequantizedbuffer[BUFFERSIZE];
struct parameters params = {8192,19660,10,15};
unsigned short valuesbuffer[10] = {0,0,0,0,0,0,0,0,0,0};
struct start_values values = {0,1,0,0,0,valuesbuffer};
int bufPos, bufIndex, read, quantPos, j;

/* This is the function that is called when the program starts. */
int main (int argc, char *argv[])
{
  /* Variable declarations. */

  memset(&input, 0, sizeof(struct wavpcm_input)); //Fill sizeof(...) bytes starting from input with
  input.resource=INPUTWAVFILE;


  /* First open input file and parse header, */
  wavpcm_input_open (&input);

  /*bufPos expressed in temporal samples*/
  for (bufPos=0; bufPos<80; bufPos+=(BUFFERSIZE/2)) {
    /* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
    /* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
    read = wavpcm_input_read (&input, buffer);
	if(read != BUFFERSIZE)
		printf("Not a full buffer read, amount read: %d" , read);

/*	printf("Input:");
        j = 0;
    	for(; j < BUFFERSIZE; j++) {
        	printf("%d ", buffer[j]);
    	}
*/
    /* transform buffer */
	for (quantPos = 0; quantPos<8; quantPos++){
        dequantize(dequantizedbuffer, *(buffer+5*quantPos), 5, &params, &values);
        j = 0;
		for(; j < BUFFERSIZE/8; j++) {
        		printf("%d ", dequantizedbuffer[j]);
        }
	}


  }

  /* Return successful exit code. */
  return 0;
}
