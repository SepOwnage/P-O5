#include "globals.h"
#include "quantizeStructs.h"
#include "dequantizeMono.h"

void dequantizeMono(short *dequantized_samples_left, short *samples_left,
                unsigned char nb_samples_to_do, struct parameters *params, struct start_values *values_left,short *dequantized_samples_right, short *samples_right,
                struct start_values *values_right){
    /*
	arguments: see dequantizeTogether.
	This version only dequantizes one channel and then copies the output to the other channel.
	*/

	// Variable declaration & initialisation
	short phi = params->phi;
	short mu = params->mu;
	unsigned char buffer_length = params->buffer_length;
	short stepsize = values_left->stepsize;
	short prediction = values_left->prediction;
	unsigned int buffersum = values_left->buffersum; //It's an int so don't use too many additions ->low samples_length
	short prev_dequantized_sample = values_left->prev_dequantized_sample;
	unsigned short * restrict buffer = values_left->buffer;
	unsigned short buffer_position_counter = values_left->buffer_position_counter;
	unsigned short * restrict bufferSamplePointer = buffer + buffer_position_counter;
	unsigned short * restrict endOfBuffer = buffer + buffer_length;
	short dequantized_difference;
	short dequantized_sample;
	unsigned char i = 0; //loop counter
	short sample;

	unsigned int stepsize_lower, stepsize_upper, buffersum_phi_product;

#pragma MUST_ITERATE(5,5,5)
	for(; i < nb_samples_to_do; i++){
        sample = *(samples_left++);

        //Dequantize
        dequantized_difference = sample * stepsize;

        //Update prediction and output dequantized sample
		dequantized_sample = (short)(dequantized_difference + prediction);
		*(dequantized_samples_left++) = dequantized_sample;
		prediction = dequantized_sample - ((mu * prev_dequantized_sample)/(1<<15));
		prev_dequantized_sample = dequantized_sample;

		//dequantized_difference = abs(dequantized_difference)
		dequantized_difference = abs(dequantized_difference);

        //Update the buffersum (=> var => stepsize ) and the buffer itself
		buffersum = buffersum - *(bufferSamplePointer) + dequantized_difference; //Update buffersum
		*(bufferSamplePointer) = dequantized_difference; //Update buffer

		buffersum_phi_product = (buffersum  * phi) >> 15;

		stepsize_lower = 1;
		stepsize_upper = 32767;
		stepsize = (stepsize_lower + stepsize_upper) >> 1; //Compiler knows this is a constant
		while (stepsize != stepsize_lower) {
			if ((buffer_length * stepsize) > buffersum_phi_product) {
				stepsize_upper = stepsize;
				stepsize = (stepsize_lower + stepsize_upper) >> 1;
			} else {
				stepsize_lower = stepsize;
				stepsize = (stepsize_lower + stepsize_upper) >> 1;
			}
		}

        //Increment the buffer_position_counter
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
