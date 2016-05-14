#include "globals.h"
#include "quantizeTogether.h"
#include <stdio.h>

void quantizeTogether(short * restrict quantized_differences_left, short * restrict start_of_samples_array_left,
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
	short difference_left;
	short difference_right;
	short quantized_difference_left;
	short quantized_difference_right;
	short dequantized_difference_left;
	short dequantized_difference_right;
	short dequantized_sample_left;
	short dequantized_sample_right;
	short stepsize_left = values_left->stepsize;
	short stepsize_right = values_right->stepsize;
	short prediction_left = values_left->prediction;
	short prediction_right = values_right->prediction;
	unsigned int buffersum_left = values_left->buffersum; //Sum of the values in the buffer, used to calculate the stepsize
	unsigned int buffersum_right = values_right->buffersum;
	short prev_dequantized_sample_left = values_left->prev_dequantized_sample;
	short prev_dequantized_sample_right = values_right->prev_dequantized_sample;
	unsigned short * restrict buffer_left = values_left->buffer;
	unsigned short * restrict buffer_right = values_right->buffer;
	unsigned short buffer_position_counter = values_left->buffer_position_counter;
	unsigned short * restrict bufferSamplePointer_left = buffer_left + buffer_position_counter;
	unsigned short * restrict bufferSamplePointer_right = buffer_right + buffer_position_counter;
	unsigned short * restrict endOfBuffer_left = buffer_left + buffer_length;
	unsigned short * restrict endOfBuffer_right = buffer_right + buffer_length;
	unsigned char i = 0; //loop counter
	short sample_left;
	short sample_right;
	short * restrict samplepointer_left = start_of_samples_array_left + start_position_in_samples_array_left;
	short * restrict samplepointer_right = start_of_samples_array_right + start_position_in_samples_array_right;
	short * restrict endOfSamplesArray_left = start_of_samples_array_left + length_of_samples_array_left;

	unsigned int stepsize_lower, stepsize_upper, buffersum_phi_product_left, buffersum_phi_product_right;

#pragma MUST_ITERATE(5,5,5)
	for (; i < nb_samples_to_do; i++) {
		sample_left = *samplepointer_left;
		sample_right = *samplepointer_right;
		samplepointer_left++;
		samplepointer_right++;
		if(samplepointer_left == endOfSamplesArray_left) //Assume this happens for the right part too
			samplepointer_left = start_of_samples_array_left;
			samplepointer_right = start_of_samples_array_right;


		//Calculate the difference between the sample and the prediction. Quantize this and write to output
		difference_left = sample_left - prediction_left;
		difference_right = sample_right - prediction_right;

		if (difference_left > 0) {
			quantized_difference_left = -1;
			while (quantized_difference_left < maximum && difference_left >= 0) {
				quantized_difference_left += 1;
				difference_left -= stepsize_left;
			}

		} else {
			quantized_difference_left = 1;
			while (quantized_difference_left > minimum && difference_left <= 0) {
				quantized_difference_left -= 1;
				difference_left += stepsize_left;
			}
		}
		if (difference_right > 0) {
			quantized_difference_right = -1;
			while (quantized_difference_right < maximum && difference_right >= 0) {
				quantized_difference_right += 1;
				difference_right -= stepsize_right;
			}

		} else {
			quantized_difference_right = 1;
			while (quantized_difference_right > minimum && difference_right <= 0) {
				quantized_difference_right -= 1;
				difference_right += stepsize_right;
			}
		}

		*(quantized_differences_left++) = quantized_difference_left;
		*(quantized_differences_right++) = quantized_difference_left;

		//Dequantize and use this parameter instead of 'difference', since the dequantizer only has access to this parameter and not to 'difference'
		dequantized_difference_left = quantized_difference_left * stepsize_left;
		dequantized_difference_right = quantized_difference_right * stepsize_right;

		//Update prediction
		dequantized_sample_left = dequantized_difference_left + prediction_left;
		dequantized_sample_right = dequantized_difference_right + prediction_right;
		prediction_left = dequantized_sample_left - (mu * prev_dequantized_sample_left)/(1<<15);
		prediction_right = dequantized_sample_right - (mu * prev_dequantized_sample_right)/(1<<15);
		prev_dequantized_sample_left = dequantized_sample_left;
		prev_dequantized_sample_right = dequantized_sample_right;

		dequantized_difference_left = abs(dequantized_difference_left);
		dequantized_difference_right = abs(dequantized_difference_right);


		//update the buffersum (=> var => stepsize ) and the buffer itself
		buffersum_left = buffersum_left - *(bufferSamplePointer_left) + dequantized_difference_left; //Update buffersum
		buffersum_right = buffersum_right - *(bufferSamplePointer_right) + dequantized_difference_right;
		*(bufferSamplePointer_left) = dequantized_difference_left; //Update buffer
		*(bufferSamplePointer_right) = dequantized_difference_right;

		buffersum_phi_product_left = (buffersum_left  * phi) >> 15;
		buffersum_phi_product_right = (buffersum_right * phi) >> 15;

		stepsize_lower = 1;
		stepsize_upper = 32767;
		stepsize_left = (stepsize_lower + stepsize_upper)>>1; //Compiler knows this is a constant
		while (stepsize_left != stepsize_lower) {
			if ((buffer_length * stepsize_left) > buffersum_phi_product_left) {
				stepsize_upper = stepsize_left;
				stepsize_left = (stepsize_lower + stepsize_upper) >> 1;
			} else {
				stepsize_lower = stepsize_left;
				stepsize_left = (stepsize_lower + stepsize_upper) >> 1;
			}
		}
		stepsize_lower = 1;
		stepsize_upper = 32767;
		stepsize_right = (stepsize_lower + stepsize_upper)>>1; //Compiler knows this is a constant
		while (stepsize_right != stepsize_lower) {
			if ((buffer_length * stepsize_right) > buffersum_phi_product_right) {
				stepsize_upper = stepsize_right;
				stepsize_right = (stepsize_lower + stepsize_upper) >> 1;
			} else {
				stepsize_lower = stepsize_right;
				stepsize_right = (stepsize_lower + stepsize_upper) >> 1;
			}
		}


		//increment the buffer position
		bufferSamplePointer_left++;
		bufferSamplePointer_right++;
		if(bufferSamplePointer_left >= endOfBuffer_left){
			bufferSamplePointer_left = buffer_left;
		}
		if(bufferSamplePointer_right >= endOfBuffer_right){
			bufferSamplePointer_right = buffer_right;
		}
	}

	//Save values to struct
	values_left->prediction = prediction_left;
	values_right->prediction = prediction_right;
	values_left->stepsize = stepsize_left;
	values_right->stepsize = stepsize_right;
	values_left->prev_dequantized_sample = prev_dequantized_sample_left;
	values_right->prev_dequantized_sample = prev_dequantized_sample_right;
	buffer_position_counter += nb_samples_to_do;
	while (buffer_position_counter >= buffer_length) //if should be sufficient with current parameters (because 5 samples per time and bufferlength 10, but while is safer if this ever gets changed_
		buffer_position_counter -= buffer_length;
	values_left->buffer_position_counter = buffer_position_counter;
	values_right->buffer_position_counter = buffer_position_counter;
	values_left->buffersum = buffersum_left;
	values_right->buffersum = buffersum_right;
}
