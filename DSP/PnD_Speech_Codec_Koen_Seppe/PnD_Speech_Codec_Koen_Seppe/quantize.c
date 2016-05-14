#include "globals.h"
#include "quantize.h"
#include <stdio.h>

void quantize(short * restrict quantized_differences, short * restrict start_of_samples_array,
	unsigned char start_position_in_samples_array, unsigned char length_of_samples_array,
	unsigned char nb_samples_to_do, struct parameters *params, struct start_values *values) {
	/*
	Quantizes the inputs with an adaptive, differential quantization scheme.
	Quantized_difference: start location of output
	start_of_samples_array: pointer to start of the array holding the samples
	start_position_in_samples_array: where in the array holding the samples the current samples are
	length_of_samples_array: the length of the array holding the samples (reading after this length=reading at the beginning)
	nb_samples_to_do: the amount of samples to process
	params: holding subband specific parameters
	values: structure containing information from previous calls of this function
	*/

	// Variable declaration & initialisation
	short phi = params->phi;
	short mu = params->mu;
	unsigned char buffer_length = params->buffer_length;
	short maximum = params->maximum;
	short minimum = -maximum -1;
	short difference;
	short quantized_difference;
	short dequantized_difference;
	short dequantized_sample;
	short stepsize = values->stepsize;
	short prediction = values->prediction;
	unsigned int buffersum = values->buffersum; //Sum of the values in the buffer, used to calculate the stepsize
	short prev_dequantized_sample = values->prev_dequantized_sample;
	unsigned short * restrict buffer = values->buffer;
	unsigned short buffer_position_counter = values->buffer_position_counter;
	unsigned short * restrict bufferSamplePointer = buffer + buffer_position_counter;
	unsigned short * restrict endOfBuffer = buffer + buffer_length;
	unsigned char i = 0; //loop counter
	short sample;
	short * restrict samplepointer = start_of_samples_array + start_position_in_samples_array;
	short * restrict endOfSamplesArray = start_of_samples_array + length_of_samples_array;

	unsigned int stepsize_lower, stepsize_upper, buffersum_phi_product;

#pragma MUST_ITERATE(5,5,5)
	for (; i < nb_samples_to_do; i++) {
		sample = *samplepointer;
		samplepointer++;
		if(samplepointer == endOfSamplesArray)
			samplepointer = start_of_samples_array;

		//Calculate the difference between the sample and the prediction. Quantize this and write to output
		difference = sample - prediction;

		if (difference > 0) {
			quantized_difference = -1;
			while (quantized_difference < maximum && difference >= 0) {
				quantized_difference += 1;
				difference -= stepsize;
			}

		} else {
			quantized_difference = 1;
			while (quantized_difference > minimum && difference <= 0) {
				quantized_difference -= 1;
				difference += stepsize;
			}
		}

		*(quantized_differences++) = (short)quantized_difference;

		//Dequantize and use this parameter instead of 'difference', since the dequantizer only has access to this parameter and not to 'difference'
		dequantized_difference = quantized_difference * stepsize;

		//Update prediction
		dequantized_sample = dequantized_difference + prediction;
		prediction = dequantized_sample - (mu * prev_dequantized_sample)/(1<<15);
		prev_dequantized_sample = dequantized_sample;

		dequantized_difference = abs(dequantized_difference);

		//update the buffersum (=> var => stepsize ) and the buffer itself
		buffersum = buffersum - *(bufferSamplePointer) + dequantized_difference; //Update buffersum
		*(bufferSamplePointer) = dequantized_difference; //Update buffer

		buffersum_phi_product = (buffersum  * phi) >> 15;

		stepsize_lower = 1;
		stepsize_upper = 32767;
		stepsize = (stepsize_lower + stepsize_upper)>>1;
		while (stepsize != stepsize_lower) {
			if ((buffer_length * stepsize) > buffersum_phi_product) {
				stepsize_upper = stepsize;
				stepsize = (stepsize_lower + stepsize_upper) >> 1;
			} else {
				stepsize_lower = stepsize;
				stepsize = (stepsize_lower + stepsize_upper) >> 1;
			}
		}


		//increment the buffer position
		bufferSamplePointer++;
		if(bufferSamplePointer >= endOfBuffer){
			bufferSamplePointer = buffer;
		}
	}

	//Save values to struct
	values->prediction = prediction;
	values->stepsize = stepsize;
	values->prev_dequantized_sample = prev_dequantized_sample;
	buffer_position_counter += nb_samples_to_do;
	while (buffer_position_counter >= buffer_length) //if should be sufficient with current parameters (because 5 samples per time and bufferlength 10, but while is safer if this ever gets changed_
		buffer_position_counter -= buffer_length;
	values->buffer_position_counter = buffer_position_counter;
	values->buffersum = buffersum;
}
