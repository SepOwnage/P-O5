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
		*((historyChunk->leftOdd) + (historyChunk->position1)) = buffer[4*i+1];
		//printf(" %d \n", *((historyChunk->leftOdd) + (historyChunk->position1)));
		//printf("%x %d ", ((historyChunk->rightEven) + (historyChunk->position1)), *((historyChunk->rightEven) + (historyChunk->position1)));
		*((historyChunk->rightEven) + (historyChunk->position1)) = buffer[4*i+2];
		//printf(" %d \n", *((historyChunk->rightEven) + (historyChunk->position1)));
		//printf("%x %d ", ((historyChunk->rightOdd) + (historyChunk->position1)), *((historyChunk->rightOdd) + (historyChunk->position1)));
		*((historyChunk->rightOdd) + (historyChunk->position1)) = buffer[4*i+3];
		//printf(" %d \n", *((historyChunk->rightOdd) + (historyChunk->position1)));
		(historyChunk->position1) = ((historyChunk->position1) +1) % (BUFFERSIZE/4 + LENGTH_FILTER1_HALF);
	}
}

void convolve(short *input, short *reversedFilter, unsigned short inputOffset, unsigned short inputL,unsigned short filterL, short *output){ //must be able to work in place? => works in place when output == input
	unsigned short i,j;
	int result;
	unsigned short stop = inputL-filterL; // 10

	for(j=0; j < stop; j++){ // 0 tem 9
		result = 0;
		unsigned short smallstop = filterL;
		for(i = 0; i<smallstop; i++){ // 1 tem 40   ....   10 tem 49
		    //printf("bufsample: %x ,  filtersample: %x \n", (input+(i+inputOffset)%inputL),(reversedFilter+i));
		    //printf("bufsample: %d ,  filtersample: %d \n", *(input+(i+inputOffset)%inputL),*(reversedFilter+i));
		    //printf("%d,%d|",i, (i+inputOffset)%inputL);
			result += (*(input+(i+inputOffset+j+1)%inputL)) * (*(reversedFilter+i));    //laatste  = input+i = input+filterL+j = input+filterL+inputL-filterL
		}
		//printf("j: %d ", j);
		*output = (short)(result>>16);
		output++;
	} 
}

void combine(short *upper, short *lower, short *out_low, short *out_high, unsigned short length){//can work in place with in and outputs the same
	//TODO: wrap around?   ==>  only if doesn't align (history (and thus filterlengths) integer multiple of buffers etc.)
	int temp;
	do{
		temp = (short)(((int) *upper + (int) *lower)>>1);
		*out_high = (short)(((int) *upper - (int) *lower)>>1);
		*out_low = temp;
		upper++;
		lower++;
		out_low++;
		out_high++;
		length--;
	}while(length > 0);
}

void copyToLowerLayer(struct chunk *historyChunk){
	int i;
	for(i=0;i < BUFFERSIZE/8;i++){   //   /8 ?/
		*(historyChunk->leftLowEven + historyChunk->position2) = 
			*(historyChunk->leftEven + historyChunk->position1 + (i<<1));
		*(historyChunk->leftLowOdd + historyChunk->position2) = 
			*(historyChunk->leftEven + historyChunk->position1 + (i<<1) +1);
		
		*(historyChunk->leftHighEven + historyChunk->position2) = 
			*(historyChunk->leftOdd + historyChunk->position1 + (i<<1));
		*(historyChunk->leftHighOdd + historyChunk->position2) = 
			*(historyChunk->leftOdd + historyChunk->position1 + (i<<1) +1);
		
		*(historyChunk->rightLowEven + historyChunk->position2) = 
			*(historyChunk->rightEven + historyChunk->position1 + (i<<1));
		*(historyChunk->rightLowOdd + historyChunk->position2) = 
			*(historyChunk->rightEven + historyChunk->position1 + (i<<1) +1);
		
		*(historyChunk->rightHighEven + historyChunk->position2) = 
			*(historyChunk->rightOdd + historyChunk->position1 + (i<<1));
		*(historyChunk->rightHighOdd + historyChunk->position2) = 
			*(historyChunk->rightOdd + historyChunk->position1 + (i<<1) +1);
		
		(historyChunk->position2) = ((historyChunk->position2)+1)%(BUFFERSIZE/8 + LENGTH_FILTER2_HALF);
	}
}

void encode(short buffer[BUFFERSIZE],struct chunk *historyChunk){
	//copy new buffer to chunk of history, overwriting oldest data that no longer matters
	addBufferToHistory(buffer, historyChunk);
	
	//convolve 4 times:  L/R and even/odd
	convolve(historyChunk->leftEven, filter1Even,
			 historyChunk->position1, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftEven) + (historyChunk->position1) );
	convolve(historyChunk->leftOdd, filter1Odd,
			 historyChunk->position1, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftOdd) + (historyChunk->position1) );
	convolve(historyChunk->rightEven, filter1Even,
			 historyChunk->position1, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->rightEven) + (historyChunk->position1) );
	convolve(historyChunk->rightOdd, filter1Odd,
			 historyChunk->position1, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->rightOdd) + (historyChunk->position1) );
	
	// do the additions and substractions of QMF
	combine((historyChunk->leftEven) + (historyChunk->position1),
			(historyChunk->leftOdd) + (historyChunk->position1),
			(historyChunk->leftEven) + (historyChunk->position1),
			(historyChunk->leftOdd) + (historyChunk->position1),
			BUFFERSIZE/4);
	combine((historyChunk->rightEven) + (historyChunk->position1),
			(historyChunk->rightOdd) + (historyChunk->position1),
			(historyChunk->rightEven) + (historyChunk->position1),
			(historyChunk->rightOdd) + (historyChunk->position1),
			BUFFERSIZE/4);
	
	//first filter depth done at this point		
	copyToLowerLayer(historyChunk);
	
	//8 times convolve
	convolve(historyChunk->leftLowEven, filter2Even,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftLowEven+historyChunk->position2);
	convolve(historyChunk->leftLowOdd, filter2Odd,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftLowOdd+historyChunk->position2);
	convolve(historyChunk->leftHighEven, filter2Even,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftHighEven+historyChunk->position2);
	convolve(historyChunk->leftHighOdd, filter2Odd,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->leftHighOdd+historyChunk->position2);

	convolve(historyChunk->rightLowEven, filter3Even,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightLowEven+historyChunk->position2);
	convolve(historyChunk->rightLowOdd, filter3Odd,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightLowOdd+historyChunk->position2);
	convolve(historyChunk->rightHighEven, filter3Even,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightHighEven+historyChunk->position2);
	convolve(historyChunk->rightHighOdd, filter3Odd,
				historyChunk->position2, BUFFERSIZE/8+LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
				historyChunk->rightHighOdd+historyChunk->position2);
	//4 times combine
	combine((historyChunk->leftLowEven) + (historyChunk->position2),
			(historyChunk->leftLowOdd) + (historyChunk->position2),
			(historyChunk->leftLowEven) + (historyChunk->position2),
			(historyChunk->leftLowOdd) + (historyChunk->position2),
			BUFFERSIZE/8);
	combine((historyChunk->leftHighEven) + (historyChunk->position2),
			(historyChunk->leftHighOdd) + (historyChunk->position2),
			(historyChunk->leftHighEven) + (historyChunk->position2),
			(historyChunk->leftHighOdd) + (historyChunk->position2),
			BUFFERSIZE/8);
	combine((historyChunk->rightLowEven) + (historyChunk->position2),
			(historyChunk->rightLowOdd) + (historyChunk->position2),
			(historyChunk->rightLowEven) + (historyChunk->position2),
			(historyChunk->rightLowOdd) + (historyChunk->position2),
			BUFFERSIZE/8);
	combine((historyChunk->rightHighEven) + (historyChunk->position2),
			(historyChunk->rightHighOdd) + (historyChunk->position2),
			(historyChunk->rightHighEven) + (historyChunk->position2),
			(historyChunk->rightHighOdd) + (historyChunk->position2),
			BUFFERSIZE/8);
	//shit is done
	
}
