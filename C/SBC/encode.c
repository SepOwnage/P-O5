#include "encode.h"
#include <stdio.h>
void addBufferToHistory(short buffer[BUFFERSIZE], struct chunk *historyChunk){
	int i;
	for(i=0;i<BUFFERSIZE/4;i++){
	    //printf("%x \n", historyChunk->leftEven);
	    //printf("%x %d ", ((historyChunk->leftEven) + (historyChunk->position1)), *((historyChunk->leftEven) + (historyChunk->position1)));
		*((historyChunk->leftEven) + (historyChunk->position1)) = buffer[4*i];
		//printf(" %d \n", *((historyChunk->leftEven) + (historyChunk->position1)));
		//printf("%x %d ", ((historyChunk->leftOdd) + (historyChunk->position1)), *((historyChunk->leftOdd) + (historyChunk->position1)));
		*((historyChunk->rightEven) + (historyChunk->position1)) = buffer[4*i+1];
		//printf(" %d \n", *((historyChunk->leftOdd) + (historyChunk->position1)));
		//printf("%x %d ", ((historyChunk->rightEven) + (historyChunk->position1)), *((historyChunk->rightEven) + (historyChunk->position1)));
		*((historyChunk->leftOdd) + (historyChunk->position1)) = buffer[4*i+2];
		//printf(" %d \n", *((historyChunk->rightEven) + (historyChunk->position1)));
		//printf("%x %d ", ((historyChunk->rightOdd) + (historyChunk->position1)), *((historyChunk->rightOdd) + (historyChunk->position1)));
		*((historyChunk->rightOdd) + (historyChunk->position1)) = buffer[4*i+3];
		//printf(" %d \n", *((historyChunk->rightOdd) + (historyChunk->position1)));
		(historyChunk->position1) = ((historyChunk->position1) +1) % (BUFFERSIZE/4 + LENGTH_FILTER1_HALF);
	}
}

void convolve(short *input, short *reversedFilter,
		unsigned short inputOffset, unsigned short inputL,
		unsigned short filterL, 
		short *output, unsigned short outputOffset, 
		unsigned short outputLength, unsigned short amountToShift){ //must be able to work in place? => works in place when output == input
	//TODO: bitshift does floor instead of to zero
	unsigned short i,j;
	long long result;
	unsigned short stop = inputL-filterL; // 10
	printf("stop: %d", stop);
	for(j=0; j < stop; j++){ // 0 tem 9
		result = 0;
		unsigned short smallstop = filterL;
		for(i = 0; i<smallstop; i++){ // 1 tem 40   ....   10 tem 49
		    //printf("j:%d i:%d bufsample: %p ,  filtersample: %p \n",j,i,(void *)  (input+(i+inputOffset+j+1)%inputL),(void *) (reversedFilter+i));
		    //printf("bufsample: %d ,  filtersample: %d \n", *(input+(i+inputOffset)%inputL),*(reversedFilter+i));
		    //printf("%d,%d|",i, (i+inputOffset)%inputL);
			result += (*(input+(i+inputOffset+j+1)%inputL)) * (*(reversedFilter+i));    //laatste  = input+i = input+filterL+j = input+filterL+inputL-filterL
		}
		result = result>>amountToShift;
		if (result> 32767)
			result = 32767;
		else if (result < -32768)
			result = -32767;
		//printf("outsample %p",(void *) ( output + (j+outputOffset)%outputLength));
		*(output + (j+outputOffset)%outputLength) = (short)(result);
	} 
}

void combine(short *upper, short *lower, short *out_low, short *out_high,
			 unsigned short length, unsigned short start, unsigned short arrayLength){
			//can work in place with in and outputs the same
	short i;	
	int temp;
	int position;
	for(i=0;i<length;i++){
		position = (start + i) % arrayLength;
		//printf("upper %d  lower  %d\n", *(upper+position), *(lower+position));
		temp = (short)(   ((int) *(upper+position) + (int) *(lower+position))  >>1);
		*(out_high+position) = (short)(   ((int) *(upper+position) - (int) *(lower+position))  >>1);
		*(out_low+position) = temp;
		
	}
}

void copyToLowerLayer(struct chunk *historyChunk){
	int i;
	for(i=0;i < BUFFERSIZE/8;i++){   //   /8 ?/
		*(historyChunk->leftLowEven + historyChunk->position2) = 
			*(historyChunk->leftEven + (historyChunk->position1 + (i*2))%(BUFFERSIZE/4+LENGTH_FILTER1_HALF));
		*(historyChunk->leftLowOdd + historyChunk->position2) = 
			*(historyChunk->leftEven + (historyChunk->position1 + (i*2)+1)%(BUFFERSIZE/4+LENGTH_FILTER1_HALF));
		
		*(historyChunk->leftHighEven + historyChunk->position2) = 
			*(historyChunk->leftOdd + (historyChunk->position1 + (i*2))%(BUFFERSIZE/4+LENGTH_FILTER1_HALF));
		*(historyChunk->leftHighOdd + historyChunk->position2) = 
			*(historyChunk->leftOdd + (historyChunk->position1 + (i*2)+1)%(BUFFERSIZE/4+LENGTH_FILTER1_HALF));
		
		*(historyChunk->rightLowEven + historyChunk->position2) = 
			*(historyChunk->rightEven + (historyChunk->position1 + (i*2))%(BUFFERSIZE/4+LENGTH_FILTER1_HALF));
		*(historyChunk->rightLowOdd + historyChunk->position2) = 
			*(historyChunk->rightEven + (historyChunk->position1 + (i*2)+1)%(BUFFERSIZE/4+LENGTH_FILTER1_HALF));
		
		*(historyChunk->rightHighEven + historyChunk->position2) = 
			*(historyChunk->rightOdd + (historyChunk->position1 + (i*2))%(BUFFERSIZE/4+LENGTH_FILTER1_HALF));
		*(historyChunk->rightHighOdd + historyChunk->position2) = 
			*(historyChunk->rightOdd + (historyChunk->position1 + (i*2)+1)%(BUFFERSIZE/4+LENGTH_FILTER1_HALF));
		
		(historyChunk->position2) = ((historyChunk->position2)+1)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF);
	}
}

void encode(short buffer[BUFFERSIZE],struct chunk *historyChunk){
	//copy new buffer to chunk of history, overwriting oldest data that no longer matters
	addBufferToHistory(buffer, historyChunk);
	
	//convolve 4 times:  L/R and even/odd
	convolve(historyChunk->leftEven, filter1Even,
			 historyChunk->position1, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftEven), (historyChunk->position1), BUFFERSIZE/4+LENGTH_FILTER1_HALF, 17 );
	convolve(historyChunk->leftOdd, filter1Odd,
			 historyChunk->position1, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftOdd), (historyChunk->position1), BUFFERSIZE/4+LENGTH_FILTER1_HALF, 17 );
	convolve(historyChunk->rightEven, filter1Even,
			 historyChunk->position1, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->rightEven), (historyChunk->position1), BUFFERSIZE/4+LENGTH_FILTER1_HALF, 17 );
	convolve(historyChunk->rightOdd, filter1Odd,
			 historyChunk->position1, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->rightOdd), (historyChunk->position1), BUFFERSIZE/4+LENGTH_FILTER1_HALF, 17 );
	
	// do the additions and substractions of QMF
	combine(historyChunk->leftEven,
			historyChunk->leftOdd,
			historyChunk->leftEven,
			historyChunk->leftOdd,
			BUFFERSIZE/4,
			historyChunk->position1,
            BUFFERSIZE/4 + LENGTH_FILTER1_HALF);
	combine(historyChunk->rightEven,
			historyChunk->rightOdd,
			historyChunk->rightEven,
			historyChunk->rightOdd,
			BUFFERSIZE/4,
			historyChunk->position1,
            BUFFERSIZE/4 + LENGTH_FILTER1_HALF);
	
	//first filter depth done at this point		
	copyToLowerLayer(historyChunk);
	
	//8 times convolve
	convolve(historyChunk->leftLowEven, filter2Even,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftLowEven, historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftLowOdd, filter2Odd,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftLowOdd, historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftHighEven, filter2Even,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftHighEven, historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, 16);
	convolve(historyChunk->leftHighOdd, filter2Odd,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftHighOdd, historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, 16);

	convolve(historyChunk->rightLowEven, filter3Even,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightLowEven, historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightLowOdd, filter3Odd,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightLowOdd, historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightHighEven, filter3Even,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightHighEven, historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, 16);
	convolve(historyChunk->rightHighOdd, filter3Odd,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightHighOdd, historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, 16);
	//4 times combine
	combine((historyChunk->leftLowEven),
			(historyChunk->leftLowOdd),
			(historyChunk->leftLowEven),
			(historyChunk->leftLowOdd),
			BUFFERSIZE/8,
			historyChunk->position2,
			BUFFERSIZE/8 + LENGTH_FILTER2_HALF);
	combine((historyChunk->leftHighEven),
			(historyChunk->leftHighOdd),
			(historyChunk->leftHighEven),
			(historyChunk->leftHighOdd),
			BUFFERSIZE/8,
			historyChunk->position2,
			BUFFERSIZE/8 + LENGTH_FILTER2_HALF);
	combine((historyChunk->rightLowEven),
			(historyChunk->rightLowOdd),
			(historyChunk->rightLowEven),
			(historyChunk->rightLowOdd),
			BUFFERSIZE/8,
			historyChunk->position2,
			BUFFERSIZE/8 + LENGTH_FILTER2_HALF);
	combine((historyChunk->rightHighEven),
			(historyChunk->rightHighOdd),
			(historyChunk->rightHighEven),
			(historyChunk->rightHighOdd),
			BUFFERSIZE/8,
			historyChunk->position2,
			BUFFERSIZE/8 + LENGTH_FILTER2_HALF);
	//shit is done
	
}
