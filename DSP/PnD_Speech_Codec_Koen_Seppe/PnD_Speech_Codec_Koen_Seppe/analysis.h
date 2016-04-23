#include "globals.h"
 
struct chunk{
	unsigned char position1;
	unsigned char position2;
	signed short leftEven[BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF];
	signed short leftOdd[BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF];
	signed short leftCombineTransfer;
	signed short rightEven[BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF];
	signed short rightOdd[BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF];
	signed short rightCombineTransfer;

	signed short leftLowEven[BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF];
	signed short leftLowOdd[BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF];   
	signed short leftLowCombineTransfer;
	signed short leftHighEven[BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF]; 
	signed short leftHighOdd[BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF];  
	signed short leftHighCombineTransfer;
	signed short rightLowEven[BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF]; //TODO: filter3?
	signed short rightLowOdd[BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF];  
	signed short rightLowCombineTransfer;
	signed short rightHighEven[BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF];
    signed short rightHighOdd[BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF]; 
	signed short rightHighCombineTransfer;
};

void analysis(short buffer[BUFFERSIZE],struct chunk * historyChunk);
void addBufferToHistory(short buffer[BUFFERSIZE], struct chunk *historyChunk);
void convolve(short *input, short *reversedFilter,
		unsigned char inputOffset, unsigned char inputL,
		unsigned char filterL,
		short *output, unsigned char outputOffset,
		unsigned char outputLength, unsigned char amountToShift);
void combine(short *upper, short *lower, short *out_low, short *out_high,
	unsigned char length, unsigned char start, unsigned char arrayLength,
	signed short *combineTransfer);
void copyToLowerLayer(struct chunk *historyChunk);


