#include "globals.h"
#include "quantize.h"
#include <stdio.h>

 //Als wij ooit clippen/overflowen, gaat de fout zich voortplanten TODO
void quantize(short *quantized_differences, short *samples, short samples_length, struct parameters *params, struct start_values *values){

	// Variable declaration & initialisation
	unsigned short phi = params->phi;
	unsigned short mu = params->mu;
	unsigned char buffer_length = params->buffer_length;
	signed int difference;
	//int dequantized_difference; //Check if has to be short TODO DEZE WORDT NIET GEBRUIKT WANT WE GEBRUIKEN DIFFERENCE, is dit niet te veel moeite om 1 int te besparen?
	signed short dequantized_sample;
	unsigned long stepsize = values->stepsize;
	unsigned short maximum = params->maximum;
	signed short prediction = values->prediction;
	unsigned int buffersum = values->buffersum; //It's an int so don't use too many additions ->low samples_length
	signed short prev_dequantized_sample = values->prev_dequantized_sample;
	// Loop
	for(unsigned char i = 0; i < samples_length; i++){

		difference = *(samples+i) - prediction; //Diff = sample - prediction
		difference = (difference/stepsize); //Reuse variable difference as quantized_difference
		if(difference > maximum){ //Clip the value of the quantized difference to the given maximum
			difference = maximum;
		}else if(difference < -(maximum+1)){
			difference = -(maximum+1);
		}
		*(quantized_differences+i) = (short) difference;
		difference = difference * stepsize; //Reuse difference as dequantized_difference
		if(difference < 0){ //dequantized_difference = abs(dequantized_difference)
			difference = difference * -1;
		}
		buffersum = buffersum - (values->buffer[i % buffer_length]) + difference; //Update buffersum
		values->buffer[i % buffer_length] = difference; //Update buffer
		
		//Update stepsize
		difference = (int) ((((long long)buffersum) / buffer_length * phi)>>15); //Reuse difference as to_trunc because it's an int
		if(difference == 0){ //max(difference,1)
			difference = 1;
		}
		stepsize = (short) difference;

		dequantized_sample = (short) (difference + prediction);
		prediction = (short) ((((int) dequantized_sample) - mu * prev_dequantized_sample)>>15);
		prev_dequantized_sample = dequantized_sample;


/*		printf("Diff:%d ", difference);
		printf("Prediction:%d ", prediction);
		printf("quantized_diff%d ", quantized_differences[i]);
		printf("Dequantized_difference:%d ", dequantized_difference);
		printf("Buffersum:%d ", buffersum);
		printf("stepsize%d ", stepsize);
		printf("dequantized_sample%d ", dequantized_sample);
		printf("prev_dequantized_sample%d\n\n ", prev_dequantized_sample);

		printf("phi%d ", phi);
		printf("buffersum%d ", buffersum);
		printf("buffer_length%d ", buffer_length);
		printf("division%d ", buffersum/buffer_length);
		printf("stepsize%d ", stepsize);
		printf("end");
	*/	

	}
	
	//Store parameters in values and params structs
	values->prediction = prediction;
	values->stepsize = stepsize;
	values->prev_dequantized_sample = prev_dequantized_sample;
	values->buffersum = buffersum;
}
