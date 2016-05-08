#include "globals.h"

inline void convolve(short *input_left, short *input_right, short *reversedFilter,
		unsigned char inputOffset, unsigned char inputL,
		unsigned char filterL,
		short *output_left, short *output_right, unsigned char outputOffset,
		unsigned char outputLength, unsigned char amountToShift){
	/*Convolves two signals:
	input: start position of the array holding the input signal
	reversedFilter: start position of the array holding the filter in reversed order!
	inputOffset: where in the array holding the input signal, the input signal starts
	inputL: the length of the array holding the input signal (reading after this length is
			done by reading the start of the array, i.e. modulo on indeces; FIFO like structure)
	filterL: the length of the filter;
	output: start position of the array in which to write the output
	outputOffset: similar to inputOffset
	outputLength: similar to inputL
	amountToShift: output is downscaled by 2^amountToShift

	Note that this function can work in place if output is input
	*/

	short temp_left[BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF];
	short temp_right[BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF];

	int i,j; //bookkeepings
	int result_left, result_right, result_left_sign, result_right_sign; //holds the temporary unscaled result
	unsigned short stop = inputL-filterL; // the amount of samples that can be calculated
	//short sample_left, sample_right;
	short * restrict samplepointer_left, * restrict samplepointer_right;
	short * restrict samplepointer_left_output, * restrict samplepointer_right_output;
	//short filterelem;
	short * restrict endOfInputArray = input_left + inputL;
	short * restrict endOfOutputArray = output_left + outputLength;
	//short *filterelempointer;
	//short *stopfilterelempointer = reversedFilter + filterL;

	short * restrict temp_left_pointer = temp_left;
	short * restrict temp_right_pointer = temp_right;

	samplepointer_left = input_left + inputOffset;
	samplepointer_right = input_right + inputOffset;
	for(i=0; i < inputL; i++){
		*temp_left_pointer = *samplepointer_left;
		*temp_right_pointer = *samplepointer_right;
		samplepointer_left++;
		samplepointer_right++;
		if(samplepointer_left >= endOfInputArray){
			samplepointer_left = input_left;
			samplepointer_right = input_right;
		}
		temp_left_pointer++;
		temp_right_pointer++;
	}
	temp_left_pointer = temp_left;
	temp_right_pointer = temp_right;

	samplepointer_left_output = output_left + outputOffset;
	samplepointer_right_output = output_right + outputOffset;

#pragma MUST_ITERATE(5,10,5)
	for(j=0; j < stop; j++){
		result_left = 0;
		result_right = 0;
		samplepointer_left = temp_left_pointer + j;
		samplepointer_right = temp_right_pointer + j;

#pragma MUST_ITERATE(16,32,16)
		for(i=0;i<filterL;i++){
			result_left += samplepointer_left[i+1] * reversedFilter[i];
			result_right += samplepointer_right[i+1] * reversedFilter[i];
		}
		//scale the result, clip it to short range if necessary (so it positives don't become
		//negatives and vice versa when converting to short).
		result_left_sign = result_left >> 31;
		result_left = ((((result_left + result_left_sign)^result_left_sign) >> amountToShift)+result_left_sign)^result_left_sign;
		result_right_sign = result_right >> 31;
		result_right = ((((result_right + result_right_sign)^result_right_sign) >> amountToShift)+result_right_sign)^result_right_sign;
		if (result_left > 32767)
			result_left = 32767;
		else if (result_left < -32768)
			result_left = -32768;

		if (result_right > 32767)
			result_right = 32767;
		else if (result_right < -32768)
			result_right = -32768;

		//write away output
		*(samplepointer_left_output++) = (short)(result_left);
		*(samplepointer_right_output++) = (short)(result_right);
		if(samplepointer_left_output == endOfOutputArray){
			samplepointer_left_output = output_left;
			samplepointer_right_output = output_right;
		}
	}
}
