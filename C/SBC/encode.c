#include "encode.h"
struct chunk{
	unsigned short position;
	signed short leftEven[BUFFERSIZE/4 + LENGTH_FILTER1_HALF];
	signed short leftOdd[BUFFERSIZE/4 + LENGTH_FILTER1_HALF];
	signed short rightEven[BUFFERSIZE/4 + LENGTH_FILTER1_HALF];
	signed short rightOdd[BUFFERSIZE/4 + LENGTH_FILTER1_HALF];
}; 

 void addBufferToHistory(short buffer[BUFFERSIZE], struct chunk *historyChunk){
	int i;
	for(i=0;i<BUFFERSIZE/4;i++){
	    //printf("%x \n", historyChunk->leftEven);
	    //printf("%x %d ", ((historyChunk->leftEven) + (historyChunk->position)), *((historyChunk->leftEven) + (historyChunk->position)));
		*((historyChunk->leftEven) + (historyChunk->position)) = buffer[4*i];
		//printf(" %d \n", *((historyChunk->leftEven) + (historyChunk->position)));
		//printf("%x %d ", ((historyChunk->leftOdd) + (historyChunk->position)), *((historyChunk->leftOdd) + (historyChunk->position)));
		*((historyChunk->leftOdd) + (historyChunk->position)) = buffer[4*i+1];
		//printf(" %d \n", *((historyChunk->leftOdd) + (historyChunk->position)));
		//printf("%x %d ", ((historyChunk->rightEven) + (historyChunk->position)), *((historyChunk->rightEven) + (historyChunk->position)));
		*((historyChunk->rightEven) + (historyChunk->position)) = buffer[4*i+2];
		//printf(" %d \n", *((historyChunk->rightEven) + (historyChunk->position)));
		//printf("%x %d ", ((historyChunk->rightOdd) + (historyChunk->position)), *((historyChunk->rightOdd) + (historyChunk->position)));
		*((historyChunk->rightOdd) + (historyChunk->position)) = buffer[4*i+3];
		//printf(" %d \n", *((historyChunk->rightOdd) + (historyChunk->position)));
		(historyChunk->position) = ((historyChunk->position) +1) % (BUFFERSIZE/4 + LENGTH_FILTER1_HALF);
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
		*output = (short)(result);
		output++;
	} 
}
void encode(short buffer[BUFFERSIZE],struct chunk *historyChunk, struct chunk *outputChunk){
	
	addBufferToHistory(buffer, historyChunk);
	convolve(historyChunk->leftEven, filter1Even,
			 historyChunk->position, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftEven) + (historyChunk->position) );
	convolve(historyChunk->leftOdd, filter1Odd,
			 historyChunk->position, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->leftOdd) + (historyChunk->position) );
	convolve(historyChunk->rightEven, filter1Even,
			 historyChunk->position, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->rightEven) + (historyChunk->position) );
	convolve(historyChunk->rightOdd, filter1Odd,
			 historyChunk->position, BUFFERSIZE/4+LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
			 (historyChunk->rightOdd) + (historyChunk->position) );
	
}


