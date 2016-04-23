#include "globals.h"

struct parameters{//Holds phi, mu, bufferlength and maximum.  This is characterised by the subband
	const unsigned short phi; //TODO check all these types
	const unsigned short mu;
	const unsigned char buffer_length;
	const unsigned short maximum;
};

struct start_values{ //Holds history between calls. This is characterised by previous samples
	signed short prediction; //Check if this can be short
	unsigned long stepsize; //Check if this can be short
	signed short prev_dequantized_sample;
	unsigned int buffersum;
	unsigned short buffer_position_counter;
	unsigned short *buffer; //* since this struct doesn't know buffer_length: Maybe this is cumbersome and both structs should be combined into one struct
};

void quantize(short *quantized_differences, short *start_of_samples_array,
	unsigned char start_position_in_samples_array, unsigned char length_of_samples_array,
	unsigned char nb_samples_to_do, const struct parameters *params, struct start_values *values);
