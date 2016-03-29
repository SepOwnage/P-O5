#include "globals.h"
#include "quantize.h"
#include <stdio.h>

 //Als wij ooit clippen/overflowen, gaat de fout zich voortplanten TODO
void quantize(short *quantized_differences, short *samples, short samples_length, struct parameters *params, struct start_values *values){

	// Variable declaration & initialisation
	unsigned short phi = params->phi;
	unsigned short mu = params->mu;
	unsigned char buffer_length = params->buffer_length;
	unsigned short maximum = params->maximum;
	int difference;
	int quantized_difference;
	int dequantized_difference;
	signed short dequantized_sample;
	unsigned long stepsize = values->stepsize;
	signed short prediction = values->prediction;
	unsigned int buffersum = values->buffersum; //It's an int so don't use too many additions ->low samples_length
	signed short prev_dequantized_sample = values->prev_dequantized_sample;
	// Loop
	for(unsigned char i = values->buffer_position_counter; i < ((values->buffer_position_counter)+samples_length); i++){	
		difference = *(samples+i) - prediction; //Diff = sample - prediction
		quantized_difference = (difference/stepsize);
		if(quantized_difference > maximum){ //Clip the value of the quantized difference to the given maximum
			quantized_difference = maximum;
		}else if(quantized_difference < -(maximum+1)){
			quantized_difference = -(maximum+1);
		}
		*(quantized_differences+i) = (short) quantized_difference;
		dequantized_difference = quantized_difference * stepsize;
		if(dequantized_difference < 0){ //dequantized_difference = abs(dequantized_difference)
			dequantized_difference = dequantized_difference * -1;
		}
		buffersum = buffersum - (values->buffer[i % buffer_length]) + dequantized_difference; //Update buffersum
		values->buffer[i % buffer_length] = dequantized_difference; //Update buffer
		stepsize = (short) ((((long long)buffersum) / buffer_length * phi)>>15);
		if(stepsize == 0){ //stepsize cannot be 0
			stepsize = 1;
		}
		dequantized_sample = (short) (difference + prediction);
		prediction = (short) (((int) dequantized_sample) - ((mu * prev_dequantized_sample)>>15));
		prev_dequantized_sample = dequantized_sample;

/*
		printf("Diff: %d ", difference);
		printf("quantized_diff: %d ", quantized_difference);
		printf("quantized_diff_maximum: %d ", quantized_difference);
		printf("Dequantized_difference: %d ", dequantized_difference);
		printf("Buffersum: %d ", buffersum);
		printf("Buffer value: %d ", values->buffer[i % buffer_length]);
		printf("stepsize: %lu ", stepsize);
		printf("dequantized_sample: %d ", dequantized_sample);
		printf("mu*prev: %d",(mu * prev_dequantized_sample));
		printf("mu*prev shifted: %d", ((mu * prev_dequantized_sample)>>15));
		printf("Prediction:%d ", prediction);
		printf("prev_dequantized_sample: %d\n\n ", prev_dequantized_sample);
*/
	}
	
	//Store parameters in values and params structs
	values->prediction = prediction;
	values->stepsize = stepsize;
	values->prev_dequantized_sample = prev_dequantized_sample;
	values->buffer_position_counter = values->buffer_position_counter + samples_length;
	while((values->buffer_position_counter) >= buffer_length){ //This is used to keep track of the index of the buffer since buffer_length can exceed samples_length
		values->buffer_position_counter = values->buffer_position_counter - buffer_length;
	}
	values->buffersum = buffersum;
}
