#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wavpcm_io.h"
#include "globals.h"

static short filter1Even[LENGTH_FILTER1_HALF] = FILTER1_EVEN;
static short filter1Odd[LENGTH_FILTER1_HALF] = FILTER1_ODD;
static short filter2Even[LENGTH_FILTER2_HALF] = FILTER2_EVEN;
static short filter2Odd[LENGTH_FILTER2_HALF] = FILTER2_ODD;
static short filter3Even[LENGTH_FILTER2_HALF] = FILTER3_EVEN;
static short filter3Odd[LENGTH_FILTER2_HALF] = FILTER3_ODD;
static short mus[4] = MUS;
static short phis[4] = PHIS;
static short buffer_lengths = BUFFER_LENGTHS;


/* This is the function that is called when the program starts. */
int main (int argc, char *argv[])
{
  /* Variable declarations. */
  struct wavpcm_input input;
  struct wavpcm_output output;
  short buffer[BUFFERSIZE]; 
  short reconstructedBuffer[BUFFERSIZE];
  int bufPos, bufIndex, read;

  memset(&input, 0, sizeof(struct wavpcm_input)); //Fill sizeof(...) bytes starting from input with
  input.resource=INPUTWAVFILE;
  memset(&output, 0, sizeof(struct wavpcm_output));
  output.resource=OUTPUTWAVFILE;

  /* First open input file and parse header, */
  wavpcm_input_open (&input);
  /* and then use this same header configuration for the output file */
  wavpcm_output_copy_settings(&input, &output);
  wavpcm_output_open(&output);
  
  /*bufPos expressed in temporal samples*/
  for (bufPos=0; bufPos<input.samplesAvailable; bufPos+=(BUFFERSIZE/2)) {
    /* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
    /* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
    read = wavpcm_input_read (&input, buffer);

    /* transform buffer */

    /* if required, dump compressed output */

    /* inverse transform buffer */
    for (bufIndex=0; bufIndex<BUFFERSIZE; bufIndex++)
      reconstructedBuffer[bufIndex]=buffer[bufIndex];

    /* dump reconstructed output */
    wavpcm_output_write (&output, reconstructedBuffer, read);
  }

  /* finalize output (write header) and close */
  wavpcm_output_close (&output);

  /* Return successful exit code. */
  return 0;
}
