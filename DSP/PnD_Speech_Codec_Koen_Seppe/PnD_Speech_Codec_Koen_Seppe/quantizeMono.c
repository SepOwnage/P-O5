#include "globals.h"
#include "quantizeStructs.h"

void quantizeMono(short * restrict quantized_differences_left, short * restrict start_of_samples_array_left,
	unsigned char start_position_in_samples_array_left, unsigned char length_of_samples_array_left,
	unsigned char nb_samples_to_do, struct parameters *params_left, struct start_values *values_left,
	short * restrict quantized_differences_right, short * restrict start_of_samples_array_right,
	unsigned char start_position_in_samples_array_right, unsigned char length_of_samples_array_right, struct start_values *values_right) {
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
		short phi = params_left->phi;
		short mu = params_left->mu;
		unsigned char buffer_length = params_left->buffer_length;
		short maximum = params_left->maximum;
		short minimum = -maximum -1;
		short difference;
		short quantized_difference;
		short dequantized_difference;
		short dequantized_sample;
		short stepsize = values_left->stepsize;
		short prediction = values_left->prediction;
		unsigned int buffersum = values_left->buffersum; //Sum of the values in the buffer, used to calculate the stepsize
		short prev_dequantized_sample = values_left->prev_dequantized_sample;
		unsigned short * restrict buffer = values_left->buffer;
		unsigned short buffer_position_counter = values_left->buffer_position_counter;
		unsigned short * restrict bufferSamplePointer = buffer + buffer_position_counter;
		unsigned short * restrict endOfBuffer = buffer + buffer_length;
		unsigned char i = 0; //loop counter
		short sample;
		short * restrict samplepointer = start_of_samples_array_left + start_position_in_samples_array_left;
		short * restrict endOfSamplesArray = start_of_samples_array_left + length_of_samples_array_left;

		unsigned int stepsize_lower, stepsize_upper, buffersum_phi_product;

#pragma MUST_ITERATE(5,5,5)
	for (; i < nb_samples_to_do; i++) {
		sample = *samplepointer;
		samplepointer++;
		if(samplepointer == endOfSamplesArray)
			samplepointer = start_of_samples_array_left;

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

		*(quantized_differences_left++) = (short)quantized_difference;

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
		stepsize = (stepsize_lower + stepsize_upper)>>1; //Compiler knows this is a constant
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
	values_left->prediction = prediction;
	values_right->prediction = prediction;
	values_left->stepsize = stepsize;
	values_right->stepsize = stepsize;
	values_left->prev_dequantized_sample = prev_dequantized_sample;
	values_right->prev_dequantized_sample = prev_dequantized_sample;
	buffer_position_counter += nb_samples_to_do;
	while (buffer_position_counter >= buffer_length) //if should be sufficient with current parameters (because 5 samples per time and bufferlength 10, but while is safer if this ever gets changed_
		buffer_position_counter -= buffer_length;
	values_left->buffer_position_counter = buffer_position_counter;
	values_right->buffer_position_counter = buffer_position_counter;
	values_left->buffersum = buffersum;
	values_right->buffersum = buffersum;
}
