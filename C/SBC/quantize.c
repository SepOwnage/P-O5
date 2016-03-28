#include "globals.h"
#include "quantize.h"
#include <stdio.h>

 //Als wij ooit clippen/overflowen, gaat de fout zich voortplanten TODO

void quantize(short *quantized_differences, short *samples, short samples_length, struct parameters *params, struct start_values *values){
	// Variable declaration & initialisation
	unsigned short phi = params->phi;
	unsigned short mu = params->mu;
	unsigned char buffer_length = params->buffer_length;
	signed short difference; //Check if has to be short
	short dequantized_difference; //Check if has to be short
	signed short dequantized_sample;
	//unsigned short buffer[buffer_length]; //Same type as quantized_differences but unsigned
	//copy(buffer,values->buffer,params->buffer_length); //TODO is het nuttig om te kopieren?
	
	
	// Loop
	for(unsigned char i = 0; i < samples_length; i++){

		difference = *(samples+i) - values->prediction;
		*(quantized_differences+i) = (short) difference/values->stepsize; //C should round this by itself
		dequantized_difference = *(quantized_differences+i) * values->stepsize;
		/*printf("quant: %hu", *(quantized_differences+i));
		printf("dequant: %hu", dequantized_difference);
		printf("stepsize: %lu\n", values->stepsize);
*/
		//TODO vs max/clipping
		values->buffersum = values->buffersum - (values->buffer[i % buffer_length])+absvalue(dequantized_difference); //The +1 in matlab code should only be there because matlab indexing starts at 1?
		values->buffer[i % buffer_length] = absvalue(dequantized_difference); //The +1 in matlab code should only be there because matlab indexing starts at 1?
		values->stepsize = max((((int)phi)*(values->buffersum)/buffer_length)>>15,1); //Mag stepsize wel short zijn?
		dequantized_sample = dequantized_difference + values->prediction;
		values->prediction = dequantized_sample - ((((int)mu) * (values->prev_dequantized_sample))>>15);
		values->prev_dequantized_sample = dequantized_sample;
	//	printf("Quantized_difference: %d", *(quantized_differences+i));

/*		printf("Diff:%d ", difference);
		printf("Prediction:%d ", values->prediction);
		printf("quantized_diff%d ", quantized_differences[i]);
		printf("Dequantized_difference:%d ", dequantized_difference);
		printf("Buffersum:%d ", values->buffersum);
		printf("stepsize%d ", values->stepsize);
		printf("dequantized_sample%d ", dequantized_sample);
		printf("prev_dequantized_sample%d\n\n ", values->prev_dequantized_sample);

		printf("phi%d ", phi);
		printf("buffersum%d ", values->buffersum);
		printf("buffer_length%d ", buffer_length);
		printf("division%d ", values->buffersum/buffer_length);
		printf("stepsize%d ", values->stepsize);
		printf("end");
	*/	

	}
	
}

/*
void copy(unsigned short *destination, unsigned short *values, unsigned char length){
	for(unsigned char i = 0; i < length; i++){
		destination[i] = values[i];
	}
}
*/

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

