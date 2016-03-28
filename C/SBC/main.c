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
short quantizedBuffer[BUFFERSIZE/8];
struct parameters params = {8192,19660,10,15};
unsigned short valuesbuffer[10] = {0,0,0,0,0,0,0,0,0,0};
struct start_values values = {0,1,0,0,valuesbuffer};
short buffer[5] = {1,2,2,5,6};

int bufPos, bufIndex, read, quantPos;

/* This is the function that is called when the program starts. */
int main (int argc, char *argv[])
{
  /* Variable declarations. */

  memset(&input, 0, sizeof(struct wavpcm_input)); //Fill sizeof(...) bytes starting from input with
  input.resource=INPUTWAVFILE;
  /* First open input file and parse header, */
  wavpcm_input_open (&input);

  /*bufPos expressed in temporal samples*/

  quantize(quantizedBuffer, buffer, 5, &params, &values);
	printf("Output:");
    	for(int j = 0; j < 5; j++) {
        	printf("%d ", quantizedBuffer[j]);
    	}



  for (bufPos=0; bufPos<40; bufPos+=(BUFFERSIZE/2)) {
    /* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
    /* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
    /*read = wavpcm_input_read (&input, buffer);
	if(read != BUFFERSIZE)
		printf("Not a full buffer read, amount read: %d" , read);
 
	printf("Input:");
    	for(int j = 0; j < BUFFERSIZE; j++) {
        	printf("%d ", buffer[j]);
    	}

*/
    /* //transform buffer 
	//encode(buffer, &theChunk);
	for (quantPos = 0; quantPos<8; quantPos++){
		quantize(quantizedbuffer, &buffer[quantPos*5], 5, &params, &values);
		printf("Next buffersize:");
		for(int j = 0; j < BUFFERSIZE/8; j++) {
        		printf("%d ", quantizedbuffer[j]);
    		}
	}          
*/
  
  }

  /* Return successful exit code. */
  return 0;
}
