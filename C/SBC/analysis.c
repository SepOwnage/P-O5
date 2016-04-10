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

void convolve(short *input, short *reversedFilter,
		unsigned short inputOffset, unsigned short inputL,
		unsigned short filterL, 
		short *output, unsigned short outputOffset, 
		unsigned short outputLength, unsigned short amountToShift){
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
	unsigned short i,j; //bookkeepings
	long long result; //holds the temporary unscaled result
	unsigned short stop = inputL-filterL; // the amount of samples that can be calculated
	short sample;
	short *samplepointer;
	short filterelem;
	short tempTestValue;
	short *endOfInputArray = input + inputL;
	short *filterelempointer;
	short *stopfilterelempointer = reversedFilter + filterL;

	for(j=0; j < stop; j++){ 
		result = 0;
		samplepointer = input + j + inputOffset;
		if (samplepointer >= endOfInputArray)
			samplepointer -= inputL;

		for(filterelempointer = reversedFilter; filterelempointer<stopfilterelempointer; filterelempointer++){
			//read data (use modulo to wrap around to beginning of array if necessary), multiply
			//with filter coeff and add to temporary result
			samplepointer++;
			if (samplepointer == endOfInputArray)
				samplepointer = input;
			sample = *samplepointer;
			filterelem = *(filterelempointer);
			result += sample * filterelem;
		}
		//scale the result, clip it to short range if necessary (so it positives don't become
		//negatives and vice versa when converting to short).
		result = result/(1<<amountToShift);
		if (result > 32767)
			result = 32767;
		else if (result < -32768)
			result = -32767;
		//write away output
		*(output + (j+outputOffset)%outputLength) = (short)(result);
	} 
}

void combine(short *upper, short *lower, short *out_low, short *out_high,
			 unsigned short length, unsigned short start, unsigned short arrayLength,
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
	position = start % arrayLength;
	uppervalue = *(upper + position);
	lowervalue = *combineTransfer;	
	*(out_low + position) = (uppervalue + lowervalue) / 2;
	highToWrite = (uppervalue - lowervalue) / 2;
	//read it already, since it might be overwritten when working in place
	lowervalue = *(lower + position);
	*(out_high + position) = highToWrite;
	
	//loop for other values
	for(i=1;i<length;i++){
		position = (start + i) % arrayLength;
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
	//encodes the new input samples, stored in buffer. Right now, only conversion to subbands is done.

	//copy new buffer to chunk of history, overwriting oldest data that no longer matters
	addBufferToHistory(buffer, historyChunk);
	
	//convolve 4 times:  L/R and even/odd
	convolve(historyChunk->leftEven, filter1Even,
			 historyChunk->position1, BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftEven), (historyChunk->position1), BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, 15 );
	convolve(historyChunk->leftOdd, filter1Odd,
			 historyChunk->position1, BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftOdd), (historyChunk->position1), BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, 15 );
	convolve(historyChunk->rightEven, filter1Even,
			 historyChunk->position1, BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->rightEven), (historyChunk->position1), BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, 15 );
	convolve(historyChunk->rightOdd, filter1Odd,
			 historyChunk->position1, BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->rightOdd), (historyChunk->position1), BUFFERSIZE_DIV4+LENGTH_FILTER1_HALF, 15 );
	
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
	convolve(historyChunk->leftLowEven, filter2Even,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftLowEven, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftLowOdd, filter2Odd,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftLowOdd, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftHighEven, filter3Even,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftHighEven, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftHighOdd, filter3Odd,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftHighOdd, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);

	convolve(historyChunk->rightLowEven, filter2Even,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightLowEven, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightLowOdd, filter2Odd,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightLowOdd, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightHighEven, filter3Even,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightHighEven, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightHighOdd, filter3Odd,
				historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightHighOdd, historyChunk->position2, BUFFERSIZE_DIV8+LENGTH_FILTER2_HALF, 15);
	//4 times combine
	//TODO: filter 3??
	//TODO: skip fourth band? => faster but should be +- neglible compared to convolve
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
