#include "globals.h"
#include "quantize.h"
#include <stdio.h>

 //Als wij ooit clippen/overflowen, gaat de fout zich voortplanten TODO
void quantize(short quantized_differences[BUFFERSIZE/8], short samples[BUFFERSIZE/8],struct parameters *params, struct start_values *values){
	// Variable declaration & initialisation
	unsigned short phi = params->phi;
	unsigned short mu = params->mu;
	unsigned char buffer_length = params->buffer_length;
	signed short difference; //Check if has to be short
	short dequantized_difference; //Check if has to be short
	signed short prediction = values->initial_prediction; //Check if has to be short (due to mu and subtraction)
	unsigned short stepsize = values->initial_stepsize; //Check if has to be short
	signed short dequantized_sample;
	signed short prev_dequantized_sample = values->prev_dequantized_sample;
	unsigned int buffersum = values->buffersum;
	unsigned short buffer[buffer_length]; //Same type as quantized_differences but unsigned
	copy(buffer,values->buffer,params->buffer_length); //TODO is het nuttig om te kopieren?
						//TODO misschien altijd in place werken in values
	
	
	// Loop
	for(unsigned char i = 0; i < BUFFERSIZE/8; i++){

		difference = samples[i] - prediction;
		quantized_differences[i] = (short) difference/stepsize; //C should round this by itself
		dequantized_difference = quantized_differences[i] * stepsize;

		buffersum = buffersum - buffer[i % buffer_length]+absvalue(dequantized_difference); //The +1 in matlab code should only be there because matlab indexing starts at 1?
		buffer[i % buffer_length] = absvalue(dequantized_difference); //The +1 in matlab code should only be there because matlab indexing starts at 1?
		stepsize = max(phi*buffersum/buffer_length,1); //Mag stepsize wel short zijn?
		dequantized_sample = dequantized_difference + prediction;
		prediction = dequantized_sample - mu * prev_dequantized_sample;
		prev_dequantized_sample = dequantized_sample;

		printf("Diff:%d ", difference);
		printf("Prediction:%d ", prediction);
		printf("quantized_diff%d ", quantized_differences[i]);
		printf("Dequantized_difference:%d ", dequantized_difference);
		printf("Buffersum:%d ", buffersum);
		printf("stepsize%d ", stepsize);
		printf("dequantized_sample%d ", dequantized_sample);
		printf("prev_dequantized_sample%d\n\n ", prev_dequantized_sample);
/*
		printf("phi%d ", phi);
		printf("buffersum%d ", buffersum);
		printf("buffer_length%d ", buffer_length);
		printf("division%d ", buffersum/buffer_length);
		printf("stepsize%d ", stepsize);
		printf("end");
	*/	

	}
	
}

void copy(unsigned short *destination, unsigned short *values, unsigned char length){
	for(unsigned char i = 0; i < length; i++){
		destination[i] = values[i];
	}
}

unsigned short absvalue(short value){

	if(value < 0){
		value = value * -1; //Will converting to short give a runtime error?
	}
	return value;
}

unsigned short max(unsigned int a,unsigned char b){ //TODO check type of a: phi*buffersum/buffer_length
	if (a > b){
		return a;
	}else{
		return b;
	}
}

