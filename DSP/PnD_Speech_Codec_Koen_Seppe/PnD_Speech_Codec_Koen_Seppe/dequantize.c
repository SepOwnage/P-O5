#include "globals.h"
#include "quantize.h"
#include "dequantize.h"
#include <stdio.h>

void dequantize(short *dequantized_samples, short *samples,
                unsigned char nb_samples_to_do, const struct parameters *params, struct start_values *values){
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
	int stepsize = values->stepsize;
	short prediction = values->prediction;
	unsigned int buffersum = values->buffersum; //It's an int so don't use too many additions ->low samples_length
	short prev_dequantized_sample = values->prev_dequantized_sample;
	unsigned short *buffer = values->buffer;
	unsigned short buffer_position_counter = values->buffer_position_counter;
    int dequantized_difference;
	short dequantized_sample;
	unsigned char i = 0; //loop counter
	short sample;

	for(; i < nb_samples_to_do; i++){
        sample = *(samples+i);

        //Dequantize
        dequantized_difference = sample * stepsize;

        //Update prediction and output dequantized sample
		dequantized_sample = (short)(dequantized_difference + prediction);
		*(dequantized_samples+i) = dequantized_sample;
		prediction = (short)(((int)dequantized_sample) - ((mu * prev_dequantized_sample)/(1<<15)));
		prev_dequantized_sample = dequantized_sample;

		//dequantized_difference = abs(dequantized_difference)
		if (dequantized_difference < 0) {
			dequantized_difference = -dequantized_difference;
		}

        //Update the buffersum (=> var => stepsize ) and the buffer itself
		buffersum = buffersum - *(buffer + buffer_position_counter) + dequantized_difference; //Update buffersum
		*(buffer + buffer_position_counter) = dequantized_difference; //Update buffer
		stepsize = (short)((((long long)buffersum) * phi / buffer_length) >> 15);
		if (!stepsize) { //stepsize cannot be 0
			stepsize = 1;
		}

        //Increment the buffer_position_counter
		buffer_position_counter = (buffer_position_counter + 1) % buffer_length;
	}

    //Save values to struct
	values->prediction = prediction;
	values->stepsize = stepsize;
	values->prev_dequantized_sample = prev_dequantized_sample;
	values->buffer_position_counter = buffer_position_counter;
	values->buffersum = buffersum;


}
