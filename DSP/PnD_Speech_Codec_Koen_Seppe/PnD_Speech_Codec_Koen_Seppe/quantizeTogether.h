#include "globals.h"

struct parameters{//Holds phi, mu, bufferlength and maximum.  This is characterised by the subband
	unsigned short phi; //TODO check all these types
	unsigned short mu;
	unsigned char buffer_length;
	unsigned short maximum;
};

struct start_values{ //Holds history between calls. This is characterised by previous samples
	signed short prediction; //Check if this can be short
	unsigned long stepsize; //Check if this can be short
	signed short prev_dequantized_sample;
	unsigned int buffersum;
	unsigned short buffer_position_counter;
	unsigned short *buffer; //* since this struct doesn't know buffer_length: Maybe this is cumbersome and both structs should be combined into one struct
};

void quantizeTogether(short * restrict quantized_differences_left, short * restrict start_of_samples_array_left,
	unsigned char start_position_in_samples_array_left, unsigned char length_of_samples_array_left,
	unsigned char nb_samples_to_do_left, struct parameters *params_left, struct start_values *values_left,
	short * restrict quantized_differences_right, short * restrict start_of_samples_array__right,
		unsigned char start_position_in_samples_array_right, unsigned char length_of_samples_array_right,
		unsigned char nb_samples_to_do_right, struct parameters *params_right, struct start_values *values_right);
