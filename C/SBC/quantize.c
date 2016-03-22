#include "globals.h"

struct parameters{ //This is characterised by the subband
	unsigned short phi; //TODO check all these types
	unsigned short mu;
	unsigned short buffer_length;
}

struct start_values{ //This is characterised by previous samples
	signed short initial_prediction;
	unsigned short initial_stepsize;
	signed short prev_dequantized_sample;
	short *buffer; //* since this struct doesn't know buffer_length: Maybe this is cumbersome and both structs should be combined into one struct
}

short *quantize(short samples[BUFFERSIZE/8],struct parameters *params, struct start_values *values){
	// Variable declaration & initialisation
	unsigned short phi = params->phi;
	unsigned short mu = params->mu;
	unsigned short buffer_length = params->buffer_length;
	signed short difference; //Check if has to be short
	short dequantized_difference; //Check if has to be short
	short quantized_differences[BUFFERSIZE/8]; ///Check if has to be short
	signed short prediction = values->initial_prediction; //Check if has to be short (due to mu and subtraction)
	unsigned short stepsize = values->initial_stepsize; //Check if has to be short
	signed short prev_dequantized_sample = values->prev_dequantized_sample;
	unsigned short buffer[buffer_length] = values->buffer[buffer_length]; //Same type as quantized_differences but unsigned. TODO: This initialisation will not work, but it is supposed to read buffer_length amount of values, starting at 
																			//*buffer from the struct, and then fill buffer[buffer_length] with these values
	
	// Loop
	for(unsigned char i = 0; i < BUFFERSIZE/8; i++){

		difference = samples[i] - prediction;
		quantized_differences[i] = (short) difference/stepsize; //C should round this by itself
		dequantized_difference = quantized_differences[i] * stepsize;

		buffersum = buffersum - buffer[mod(i,buffer_length)]+abs(dequantized_difference); //The +1 in matlab code should only be there because matlab indexing starts at 1?
		buffer[mod(i,buffer_length)] = abs(dequantized_difference); //The +1 in matlab code should only be there because matlab indexing starts at 1?
		stepsize = max(phi*buffersum/buffer_length,1);
		dequantized_sample = dequantized_difference + prediction;
		prediction = dequantized_sample - mu * prev_dequantized_sample
		prev_dequantized_sample = dequantized_sample

	}

	return quantized_differences;
	
}

unsigned char mod(unsigned char number, unsigned short modulo){ //Type should be same as buffer
	while(number >= modulo){
		number -= modulo;
	}
	return number;
}


