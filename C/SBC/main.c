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
short quantizedbuffer[BUFFERSIZE];
struct parameters params = {8192,19660,10};
unsigned short valuesbuffer[10] = {0,0,0,0,0,0,0,0,0,0};
struct start_values values = {0,1,0,0,valuesbuffer};

struct chunk theChunk = {0,0,{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};
int bufPos, bufIndex, read, quantPos;

/* This is the function that is called when the program starts. */
int main (int argc, char *argv[])
{
  /* Variable declarations. */

  memset(&input, 0, sizeof(struct wavpcm_input)); //Fill sizeof(...) bytes starting from input with
  input.resource=INPUTWAVFILE;


  /* First open input file and parse header, */
  wavpcm_input_open (&input);
  /* and then use this same header configuration for the output file 
  wavpcm_output_copy_settings(&input, &output_LL);
  (output_LL.samplingRate)/=4;
  */

  /*bufPos expressed in temporal samples*/
  for (bufPos=0; bufPos<40; bufPos+=(BUFFERSIZE/2)) {
    /* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
    /* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
    read = wavpcm_input_read (&input, buffer);
	if(read != BUFFERSIZE)
		printf("Not a full buffer read, amount read: %d" , read);
 
	printf("Input:");
    	for(int j = 0; j < BUFFERSIZE; j++) {
        	printf("%d ", buffer[j]);
    	}
    /* transform buffer */
	//encode(buffer, &theChunk);
	for (quantPos = 0; quantPos<8; quantPos++){
		quantize(quantizedbuffer, &buffer[quantPos*5], 5, &params, &values);
		printf("Next buffersize:");
		for(int j = 0; j < BUFFERSIZE/8; j++) {
        		printf("%d ", quantizedbuffer[j]);
    		}
	}          

    /* if required, dump compressed output */

    /* inverse transform buffer 
    for (bufIndex=0; bufIndex<BUFFERSIZE/8; bufIndex++){
	reconstructedBuffer_LL[2*bufIndex]=theChunk.leftLowEven[(bufIndex+theChunk.position2)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF)];
	reconstructedBuffer_LL[2*bufIndex+1]=theChunk.rightLowEven[(bufIndex+theChunk.position2)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF)];
	reconstructedBuffer_LH[2*bufIndex]=theChunk.leftLowOdd[(bufIndex+theChunk.position2)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF)];
	reconstructedBuffer_LH[2*bufIndex+1]=theChunk.rightLowOdd[(bufIndex+theChunk.position2)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF)];
	reconstructedBuffer_HL[2*bufIndex]=theChunk.leftHighEven[(bufIndex+theChunk.position2)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF)];
	reconstructedBuffer_HL[2*bufIndex+1]=theChunk.rightHighEven[(bufIndex+theChunk.position2)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF)];
	reconstructedBuffer_HH[2*bufIndex]=theChunk.leftHighOdd[(bufIndex+theChunk.position2)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF)];
	reconstructedBuffer_HH[2*bufIndex+1]=theChunk.rightHighOdd[(bufIndex+theChunk.position2)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF)];
    }*/
    /* dump reconstructed output 
    wavpcm_output_write (&output_LL, reconstructedBuffer_LL, read/4);
    wavpcm_output_write (&output_LH, reconstructedBuffer_LH, read/4);
    wavpcm_output_write (&output_HL, reconstructedBuffer_HL, read/4);
    wavpcm_output_write (&output_HH, reconstructedBuffer_HH, read/4);
    */
  }

  /* finalize output (write header) and close 
  wavpcm_output_close (&output_LL);
  wavpcm_output_close (&output_LH);
  wavpcm_output_close (&output_HL);
  wavpcm_output_close (&output_HH);
  */

  /* Return successful exit code. */
  return 0;
}
