
#ifndef QUANTIZE_STRUCTS_DEFINED
#define QUANTIZE_STRUCTS_DEFINED
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
#endif
