#include "globals.h"
 
struct chunk{
	unsigned short position1;
	unsigned short position2;
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
		unsigned short inputOffset, unsigned short inputL,
		unsigned short filterL, 
		short *output, unsigned short outputOffset, 
		unsigned short outputLength, unsigned short amountToShift);
void convolveFilter2Even(short *input,
		unsigned short inputOffset, unsigned short inputL,
		short *output, unsigned short outputOffset,
		unsigned short outputLength, unsigned short amountToShift);
void convolveFilter2Odd(short *input,
		unsigned short inputOffset, unsigned short inputL,
		short *output, unsigned short outputOffset,
		unsigned short outputLength, unsigned short amountToShift);
void convolveFilter3Even(short *input,
		unsigned short inputOffset, unsigned short inputL,
		short *output, unsigned short outputOffset,
		unsigned short outputLength, unsigned short amountToShift);
void convolveFilter3Odd(short *input,
		unsigned short inputOffset, unsigned short inputL,
		short *output, unsigned short outputOffset,
		unsigned short outputLength, unsigned short amountToShift);

void combine(short *upper, short *lower, short *out_low, short *out_high,
	unsigned short length, unsigned short start, unsigned short arrayLength,
	signed short *combineTransfer);
void copyToLowerLayer(struct chunk *historyChunk);


