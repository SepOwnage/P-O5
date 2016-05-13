#include "globals.h"
#include "quantize.h"
#include "dequantize.h"
#include <stdio.h>

void dequantize(short *dequantized_samples, short *samples,
                unsigned char nb_samples_to_do, struct parameters *params, struct start_values *values){
    /*
	Is the inverse of quantize. Dequantizes the inputs with an adaptive, differential dequantization scheme.
	Dequantized_difference: start location of output
	Samples: pointer to the array containing the samples to be dequantized
	nb_samples_to_do: the amount of samples to process
	params: holding subband specific parameters
    values: structure containing information from previous calls of this function
	*/

	// Variable declaration & initialisation
	short phi = params->phi;
	short mu = params->mu;
	unsigned char buffer_length = params->buffer_length;
	short stepsize = values->stepsize;
	short prediction = values->prediction;
	unsigned int buffersum = values->buffersum; //It's an int so don't use too many additions ->low samples_length
	short prev_dequantized_sample = values->prev_dequantized_sample;
	unsigned short * /*restrict*/ buffer = values->buffer;
	unsigned short buffer_position_counter = values->buffer_position_counter;
	unsigned short * /*restrict*/ bufferSamplePointer = buffer + buffer_position_counter;
	unsigned short * /*restrict*/ endOfBuffer = buffer + buffer_length;
	short dequantized_difference;
	short dequantized_sample;
	unsigned char i = 0; //loop counter
	short sample;

	unsigned int stepsize_lower, stepsize_upper, buffersum_phi_product;

	for(; i < nb_samples_to_do; i++){
        sample = *(samples++);

        //Dequantize
        dequantized_difference = sample * stepsize;

        //Update prediction and output dequantized sample
		dequantized_sample = (short)(dequantized_difference + prediction);
		*(dequantized_samples++) = dequantized_sample;
		prediction = dequantized_sample - ((mu * prev_dequantized_sample)/(1<<15));
		prev_dequantized_sample = dequantized_sample;

		//dequantized_difference = abs(dequantized_difference)
		if (dequantized_difference < 0) {
			dequantized_difference = -dequantized_difference;
		}

        //Update the buffersum (=> var => stepsize ) and the buffer itself
		buffersum = buffersum - *(bufferSamplePointer) + dequantized_difference; //Update buffersum
		*(bufferSamplePointer) = dequantized_difference; //Update buffer

		buffersum_phi_product = (buffersum  * phi) >> 15;

		stepsize_lower = 1;
		stepsize_upper = 32767;
		stepsize = (stepsize_lower + stepsize_upper) >> 1;
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
	values->prediction = prediction;
	values->stepsize = stepsize;
	values->prev_dequantized_sample = prev_dequantized_sample;
	buffer_position_counter += nb_samples_to_do;
	while (buffer_position_counter >= buffer_length) //if should be sufficient with current parameters (because 5 samples per time and bufferlength 10, but while is safer if this ever gets changed_
		buffer_position_counter -= buffer_length;
	values->buffer_position_counter = buffer_position_counter;
	values->buffersum = buffersum;


}
