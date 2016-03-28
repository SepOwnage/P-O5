#include "globals.h"

struct parameters{ //This is characterised by the subband
	unsigned short phi; //TODO check all these types
	unsigned short mu;
	unsigned char buffer_length;
	unsigned short maximum;
};

struct start_values{ //This is characterised by previous samples
	signed short prediction; //Check if this can be short
	unsigned long stepsize; //Check if this can be short
	signed short prev_dequantized_sample;
	unsigned int buffersum;
	unsigned short *buffer; //* since this struct doesn't know buffer_length: Maybe this is cumbersome and both structs should be combined into one struct
};

void quantize(short *quantized_differences, short *samples, short samples_length,struct parameters *params, struct start_values *values);
void copy(unsigned short *destination, unsigned short *values, unsigned char length);
unsigned short absvalue(short value);
unsigned short max(unsigned int a,unsigned char b);
