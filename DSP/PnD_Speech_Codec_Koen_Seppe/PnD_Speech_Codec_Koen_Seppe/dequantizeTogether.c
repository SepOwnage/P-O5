#include "globals.h"
#include "quantizeStructs.h"
#include "dequantizeTogether.h"
void dequantizeTogether(short *dequantized_samples_left, short *samples_left,
        unsigned char nb_samples_to_do, struct parameters *params_left, struct start_values *values_left,short *dequantized_samples_right, short *samples_right,
        struct start_values *values_right){
    /*
	Is the inverse of quantize. Dequantizes the inputs with an adaptive, differential dequantization scheme.
	Dequantized_difference: start location of output
	Samples: pointer to the array containing the samples to be dequantized
	nb_samples_to_do: the amount of samples to process
	params: holding subband specific parameters
    values: structure containing information from previous calls of this function
	*/

	// Variable declaration & initialisation
	short phi = params_left->phi;
	short mu = params_left->mu;
	unsigned char buffer_length = params_left->buffer_length;
	short stepsize_left = values_left->stepsize;
	short stepsize_right = values_right->stepsize;
	short prediction_left = values_left->prediction;
	short prediction_right = values_right->prediction;
	unsigned int buffersum_left = values_left->buffersum; //It's an int so don't use too many additions ->low samples_length
	unsigned int buffersum_right = values_right->buffersum;
	short prev_dequantized_sample_left = values_left->prev_dequantized_sample;
	short prev_dequantized_sample_right = values_left->prev_dequantized_sample;
	unsigned short * restrict buffer_left = values_left->buffer;
	unsigned short * restrict buffer_right = values_right->buffer;
	unsigned short buffer_position_counter_left = values_left->buffer_position_counter;
	unsigned short buffer_position_counter_right = values_right->buffer_position_counter;
	unsigned short * restrict bufferSamplePointer_left = buffer_left + buffer_position_counter_left;
	unsigned short * restrict bufferSamplePointer_right = buffer_right + buffer_position_counter_right;
	unsigned short * restrict endOfBuffer_left = buffer_left + buffer_length;
	unsigned short * restrict endOfBuffer_right = buffer_right + buffer_length;
	short dequantized_difference_left;
	short dequantized_difference_right;
	short dequantized_sample_left;
	short dequantized_sample_right;
	unsigned char i = 0; //loop counter
	short sample_left;
	short sample_right;

	unsigned int stepsize_lower, stepsize_upper, buffersum_phi_product_left, buffersum_phi_product_right;

#pragma MUST_ITERATE(5,5,5)
	for(; i < nb_samples_to_do; i++){
        sample_left = *(samples_left++);
        sample_right = *(samples_right++);

        //Dequantize
        dequantized_difference_left = sample_left * stepsize_left;
        dequantized_difference_right = sample_right * stepsize_right;

        //Update prediction and output dequantized sample
		dequantized_sample_left = dequantized_difference_left + prediction_left;
		dequantized_sample_right = dequantized_difference_right + prediction_right;
		*(dequantized_samples_left++) = dequantized_sample_left;
		*(dequantized_samples_right++) = dequantized_sample_right;
		prediction_left = dequantized_sample_left - ((mu * prev_dequantized_sample_left)/(1<<15));
		prediction_right = dequantized_sample_right - ((mu * prev_dequantized_sample_right)/(1<<15));
		prev_dequantized_sample_left = dequantized_sample_left;
		prev_dequantized_sample_right = dequantized_sample_right;

		//dequantized_difference = abs(dequantized_difference)
		dequantized_difference_left = abs(dequantized_difference_left);
		dequantized_difference_right = abs(dequantized_difference_right);

        //Update the buffersum (=> var => stepsize ) and the buffer itself
		buffersum_left = buffersum_left - *(bufferSamplePointer_left) + dequantized_difference_left; //Update buffersum
		buffersum_right = buffersum_right - *(bufferSamplePointer_right) + dequantized_difference_right;
		*(bufferSamplePointer_left) = dequantized_difference_left; //Update buffer
		*(bufferSamplePointer_right) = dequantized_difference_right;

		buffersum_phi_product_left = (buffersum_left  * phi) >> 15;
		buffersum_phi_product_right = (buffersum_right * phi) >> 15;

		stepsize_lower = 1;
		stepsize_upper = 32767;
		stepsize_left = (stepsize_lower + stepsize_upper) >> 1; //Compiler knows this is a constant
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
		stepsize_right = (stepsize_lower + stepsize_upper) >> 1; //Compiler knows this is a constant
		while (stepsize_right != stepsize_lower) {
			if ((buffer_length * stepsize_right) > buffersum_phi_product_right) {
				stepsize_upper = stepsize_right;
				stepsize_right = (stepsize_lower + stepsize_upper) >> 1;
			} else {
				stepsize_lower = stepsize_right;
				stepsize_right = (stepsize_lower + stepsize_upper) >> 1;
			}
		}

        //Increment the buffer_position_counter
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
	buffer_position_counter_left += nb_samples_to_do;
	buffer_position_counter_right += nb_samples_to_do;
	while (buffer_position_counter_left >= buffer_length) //if should be sufficient with current parameters (because 5 samples per time and bufferlength 10, but while is safer if this ever gets changed_
		buffer_position_counter_left -= buffer_length;
	while(buffer_position_counter_right >= buffer_length)
		buffer_position_counter_right -= buffer_length;
	values_left->buffer_position_counter = buffer_position_counter_left;
	values_right->buffer_position_counter = buffer_position_counter_right;
	values_left->buffersum = buffersum_left;
	values_right->buffersum = buffersum_right;


}
