#include "analysis.h"
#include <stdio.h>
void addBufferToHistory(short buffer[BUFFERSIZE], struct chunk *historyChunk){
	/*
	Takes an input buffer 'buffer' and copy the values in it to the corresponding arrays in historyChunk
	at position historyChunk -> position1.  historyChunk->position1 is moved to position after copied values.
	Note that if the end of the arrays in historyChunk is reached, writing will continue at the beginning of the array.
	(FIFO like structure);
	*/
	int i;
	for(i=0;i<BUFFERSIZE_DIV4;i++){
		*((historyChunk->leftEven) + (historyChunk->position1)) = buffer[4*i];		
		*((historyChunk->rightEven) + (historyChunk->position1)) = buffer[4*i+1];
		*((historyChunk->leftOdd) + (historyChunk->position1)) = buffer[4*i+2];
		*((historyChunk->rightOdd) + (historyChunk->position1)) = buffer[4*i+3];
		(historyChunk->position1) = ((historyChunk->position1) +1) % (BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF);
	}
}
//unsigned char somespace = 5;
//#pragma DATA_ALIGN(temp_left,2)
//#pragma DATA_ALIGN(temp_right,2)
short temp_left[BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF] = {0};
short temp_right[BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF] = {0};


void convolve(short *input_left, short *input_right, short *reversedFilter,
		unsigned char inputOffset, unsigned char inputL,
		unsigned char filterL,
		short *output_left, short *output_right, unsigned char outputOffset,
		unsigned char outputLength, unsigned char amountToShift){
	/*Convolves two signals:
	input_left: start position of the array holding the left channel input signal
	input_right: start position of the array holding the right channel input signal
	reversedFilter: start position of the array holding the filter in reversed order!
	inputOffset: where in the array holding the input signal, the input signal starts
	inputL: the length of the array holding the input signal (reading after this length is
			done by reading the start of the array, i.e. modulo on indeces; FIFO like structure)
	filterL: the length of the filter;
	output_left: start position of the array in which to write the left channel output
	output_right: start position of the array in which to write the right channel output
	outputOffset: similar to inputOffset
	outputLength: similar to inputL
	amountToShift: output is downscaled by 2^amountToShift

	Note that this function can work in place if output is input
	*/
	short i,j; //bookkeepings
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
	//somespace+=1;
	samplepointer_left = input_left + inputOffset;
	samplepointer_right = input_right + inputOffset;

	//just a copy loop. This gets rid of the index wrapping in the inner loop at the expense of a bit more memory use
#pragma MUST_ITERATE(21,42,21)
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
		samplepointer_left = temp_left_pointer + j+1;
		samplepointer_right = temp_right_pointer + j+1;

#pragma MUST_ITERATE(16,32,16)
#pragma UNROLL(4)
		for(i=0;i<filterL;i++){
			result_left += samplepointer_left[i] * reversedFilter[i];
			result_right += samplepointer_right[i] * reversedFilter[i];
		}
		//scale the result, clip it to short range if necessary (so it positives don't become
		//negatives and vice versa when converting to short).
		result_left_sign = result_left >> 31;
		result_left = ((((result_left + result_left_sign)^result_left_sign) >> amountToShift)+result_left_sign)^result_left_sign;
		result_right_sign = result_right >> 31;
		result_right = ((((result_right + result_right_sign)^result_right_sign) >> amountToShift)+result_right_sign)^result_right_sign;
		//result_left = _spack2(result_left, result_right); //This intrinsic makes it slower? why?

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
		if(samplepointer_left_output == endOfOutputArray){//check output index needs to wrap
			samplepointer_left_output = output_left;
			samplepointer_right_output = output_right;
		}
	}
}

void combine(short *upper, short *lower, short *out_low, short *out_high,
			 unsigned char length, unsigned char start, unsigned char arrayLength,
			 signed short *combineTransfer){
	/*
	combines upper and lower signals in QMF scheme to low and high frequency signals.
		low_frequency = upper + lower
		high_frequency = upper - lower
	upper: start of array holding the upper signal
	lower: start of array holding the lower signal
	out_low: start of array in which low frequency band should be written
	out_high: start of array in which high frequency band should be written
	length: the amount of samples to process
	start: where in the arrays the current data starts and should be written
	arrayLength: the length of previous array. Writing or reading after this length 
			corresponds to writing reading in the beginning of them (modulo on index, FIFO like)
	combineTransfer: holds the last value of the lower signal from the previous call. This is to 
					 implement the one sample delay on the lower signal. The last value of the
					 current lower signal will be saved here  (just initialize the value at
					 combineTransfer to 0 and then keep passing the same combineTransfer).
	Can work in place
	*/
	short i;
	//temporary value holding the value to write in the high frequency output
	short highToWrite;
	//temporary values holding a sample from the upper and lower signal
	int uppervalue, lowervalue;
	//the current position in the arrays
	short position;
	//first one using combineTransfer
	position = start;
	uppervalue = *(upper + position);
	lowervalue = *combineTransfer;	
	*(out_low + position) = (uppervalue + lowervalue) / 2;
	highToWrite = (uppervalue - lowervalue) / 2;
	//read it already, since it might be overwritten when working in place
	lowervalue = *(lower + position);
	*(out_high + position) = highToWrite;
	
#pragma MUST_ITERATE(5,10,5)
	//loop for other values
	for(i=1;i<length;i++){
		position++;
		if (position == arrayLength)
			position = 0;
		uppervalue = *(upper + position);
		*(out_low + position) = (uppervalue + lowervalue) / 2;
		highToWrite = (uppervalue - lowervalue) / 2;
		lowervalue = *(lower + position);
		*(out_high + position) = highToWrite;
	}
	//save the last value of the lower signal for the next call
	*combineTransfer = lowervalue;
}

void copyToLowerLayer(struct chunk *historyChunk){
	/*
	Takes the samples from depth 1 and writes them to appropriate arrays of depth 2
	Using the FIFO structures to wrap around the endings as always. position1 is not changed,
	position2 is set after the newly written data
	*/
	int i;

	for(i=0;i < BUFFERSIZE_DIV8;i++){   //   /8 ?/
		*(historyChunk->leftLowEven + historyChunk->position2) = 
			*(historyChunk->leftEven + (historyChunk->position1 + (i*2))%(BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF));
		*(historyChunk->leftLowOdd + historyChunk->position2) = 
			*(historyChunk->leftEven + (historyChunk->position1 + (i*2)+1)%(BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF));
		
		*(historyChunk->leftHighEven + historyChunk->position2) = 
			*(historyChunk->leftOdd + (historyChunk->position1 + (i*2))%(BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF));
		*(historyChunk->leftHighOdd + historyChunk->position2) = 
			*(historyChunk->leftOdd + (historyChunk->position1 + (i*2)+1)%(BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF));
		
		*(historyChunk->rightLowEven + historyChunk->position2) = 
			*(historyChunk->rightEven + (historyChunk->position1 + (i*2))%(BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF));
		*(historyChunk->rightLowOdd + historyChunk->position2) = 
			*(historyChunk->rightEven + (historyChunk->position1 + (i*2)+1)%(BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF));
		
		*(historyChunk->rightHighEven + historyChunk->position2) = 
			*(historyChunk->rightOdd + (historyChunk->position1 + (i*2))%(BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF));
		*(historyChunk->rightHighOdd + historyChunk->position2) = 
			*(historyChunk->rightOdd + (historyChunk->position1 + (i*2)+1)%(BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF));
		
		(historyChunk->position2) = ((historyChunk->position2)+1)%(BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF);
	}
}

void analysis(short buffer[BUFFERSIZE],struct chunk *historyChunk){
	//Encodes the new input samples, stored in buffer. Only conversion to subbands is done here. The output is written
	//to arrays left_low_even, left_low_odd, left_high_even, left_high_odd, right_low_even, right_low_odd, right_high_even and right_high odd.
	//chunk->position 2 indicates the position of the output in these arrays


	//copy new buffer to chunk of history, overwriting oldest data that no longer matters
	addBufferToHistory(buffer, historyChunk);
	
	//convolve 4 times:  L/R and even/odd
	convolve(historyChunk->leftEven,historyChunk->rightEven, filter1Even,
			 historyChunk->position1, BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftEven),(historyChunk->rightEven), (historyChunk->position1), BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, 15 );
	convolve(historyChunk->leftOdd,historyChunk->rightOdd, filter1Odd,
			 historyChunk->position1, BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftOdd),(historyChunk->rightOdd), (historyChunk->position1), BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, 15 );

	
	// do the additions and substractions of QMF
	combine(historyChunk->leftEven,
			historyChunk->leftOdd,
			historyChunk->leftEven,
			historyChunk->leftOdd,
			BUFFERSIZE_DIV4,
			historyChunk->position1,
            BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF,
		    &(historyChunk->leftCombineTransfer));
	combine(historyChunk->rightEven,
			historyChunk->rightOdd,
			historyChunk->rightEven,
			historyChunk->rightOdd,
			BUFFERSIZE_DIV4,
			historyChunk->position1,
            BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF,
			&(historyChunk->rightCombineTransfer));
	
	//first filter depth done at this point		
	copyToLowerLayer(historyChunk);
	
	//8 times convolve
	convolve(historyChunk->leftLowEven,historyChunk->rightLowEven, filter2Even,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftLowEven,historyChunk->rightLowEven, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftLowOdd,historyChunk->rightLowOdd, filter2Odd,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftLowOdd,historyChunk->rightLowOdd, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftHighEven,historyChunk->rightHighEven, filter3Even,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftHighEven,historyChunk->rightHighEven, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftHighOdd, historyChunk->rightHighOdd, filter3Odd,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftHighOdd,historyChunk->rightHighOdd, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	//4 times combine


	combine((historyChunk->leftLowEven),
			(historyChunk->leftLowOdd),
			(historyChunk->leftLowEven),
			(historyChunk->leftLowOdd),
			BUFFERSIZE_DIV8,
			historyChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			&(historyChunk->leftLowCombineTransfer));
	combine((historyChunk->leftHighEven),
			(historyChunk->leftHighOdd),
			(historyChunk->leftHighEven),
			(historyChunk->leftHighOdd),
			BUFFERSIZE_DIV8,
			historyChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			&(historyChunk->leftHighCombineTransfer));
	combine((historyChunk->rightLowEven),
			(historyChunk->rightLowOdd),
			(historyChunk->rightLowEven),
			(historyChunk->rightLowOdd),
			BUFFERSIZE_DIV8,
			historyChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			&(historyChunk->rightLowCombineTransfer));
	combine((historyChunk->rightHighEven),
			(historyChunk->rightHighOdd),
			(historyChunk->rightHighEven),
			(historyChunk->rightHighOdd),
			BUFFERSIZE_DIV8,
			historyChunk->position2,
			BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF,
			&(historyChunk->rightHighCombineTransfer));
	//shit is done
	
}
