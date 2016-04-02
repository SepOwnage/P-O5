#include "globals.h"
#include "quantize.h"
#include <stdio.h>

void quantize(short *quantized_differences, short *start_of_samples_array,
	short start_position_in_samples_array, short length_of_samples_array,
	short nb_samples_to_do, struct parameters *params, struct start_values *values) {
	/*
	Quantized_difference: start location of output
	start_of_samples_array: pointer to start of the array holding the samples
	start_position_in_samples_array: where in the array holding the samples the current samples are
	length_of_samples_array: the length of the array holding the samples (reading after this length=reading at the beginning)
	nb_samples_to_do: the amount of samples to process
	params: holding subband specific paramters
	values: from previous calls, holding the history.
	*/
	// Variable declaration & initialisation
	short phi = params->phi;
	short mu = params->mu;
	unsigned char buffer_length = params->buffer_length;
	short maximum = params->maximum;
	int difference;
	int quantized_difference;
	int dequantized_difference;
	short dequantized_sample;
	int stepsize = values->stepsize;
	short prediction = values->prediction;
	unsigned int buffersum = values->buffersum; //It's an int so don't use too many additions ->low samples_length
	short prev_dequantized_sample = values->prev_dequantized_sample;
	unsigned short *buffer = values->buffer;
	unsigned short buffer_position_counter = values->buffer_position_counter;
	unsigned char i = 0; //loop counter
	short sample;

	for (; i < nb_samples_to_do; i++) {
		sample = *(start_of_samples_array + (start_position_in_samples_array + i) % length_of_samples_array);


		//Diff = sample - prediction.  Quantize it and write to output
		difference = sample - prediction;
		quantized_difference = (difference / stepsize);
		if (quantized_difference > maximum) { //Clip the value of the quantized difference to the given maximum
			quantized_difference = maximum;
		}
		else if (quantized_difference < -(maximum + 1)) {
			quantized_difference = -(maximum + 1);
		}
		*(quantized_differences + i) = (short)quantized_difference;
		//dequantize
		dequantized_difference = quantized_difference * stepsize;

		//update prediction
		dequantized_sample = (short)(dequantized_difference + prediction);
		prediction = (short)(((int)dequantized_sample) - ((mu * prev_dequantized_sample)/(1<<15)));
		prev_dequantized_sample = dequantized_sample;

		//take absolute value
		if (dequantized_difference < 0) {
			dequantized_difference = -dequantized_difference;
		}

		//update the buffersum (=> var => stepsize ) and the buffer itself
		buffersum = buffersum - *(buffer + buffer_position_counter) + dequantized_difference; //Update buffersum
		*(buffer + buffer_position_counter) = dequantized_difference; //Update buffer
		stepsize = (short)((((long long)buffersum) * phi / buffer_length) >> 15);
		if (!stepsize) { //stepsize cannot be 0
			stepsize = 1;
		}

		//increment the buffer_position_counter
		buffer_position_counter = (buffer_position_counter + 1) % buffer_length;
	}

	//restore values to struct
	values->prediction = prediction;
	values->stepsize = stepsize;
	values->prev_dequantized_sample = prev_dequantized_sample;
	values->buffer_position_counter = buffer_position_counter;
	values->buffersum = buffersum;
}
