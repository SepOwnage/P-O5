#include "analysis.h"
#include <stdio.h>
#include "convolve.c"
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
	//encodes the new input samples, stored in buffer. Right now, only conversion to subbands is done.

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
