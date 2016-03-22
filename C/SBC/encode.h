#include "globals.h"
 
struct chunk{
	unsigned short position1;
	unsigned short position2;
	signed short leftEven[BUFFERSIZE/4 + LENGTH_FILTER1_HALF];
	signed short leftOdd[BUFFERSIZE/4 + LENGTH_FILTER1_HALF];
	signed short rightEven[BUFFERSIZE/4 + LENGTH_FILTER1_HALF];
	signed short rightOdd[BUFFERSIZE/4 + LENGTH_FILTER1_HALF];
	
	signed short leftLowEven[BUFFERSIZE/8 + LENGTH_FILTER2_HALF];
	signed short leftLowOdd[BUFFERSIZE/8 + LENGTH_FILTER2_HALF];   
	signed short leftHighEven[BUFFERSIZE/8 + LENGTH_FILTER2_HALF]; 
	signed short leftHighOdd[BUFFERSIZE/8 + LENGTH_FILTER2_HALF];  
	signed short rightLowEven[BUFFERSIZE/8 + LENGTH_FILTER2_HALF]; //TODO: filter3?
	signed short rightLowOdd[BUFFERSIZE/8 + LENGTH_FILTER2_HALF];  
    signed short rightHighEven[BUFFERSIZE/8 + LENGTH_FILTER2_HALF];
    signed short rightHighOdd[BUFFERSIZE/8 + LENGTH_FILTER2_HALF]; 
};

void encode(short buffer[BUFFERSIZE],struct chunk * historyChunk);
void addBufferToHistory(short buffer[BUFFERSIZE], struct chunk *historyChunk);
void convolve(short *input, short *reversedFilter, unsigned short inputOffset, unsigned short inputL,unsigned short filterL, short *output);
void combine(short *upper, short *lower, short *out_low, short *out_high, unsigned short length);
void copyToLowerLayer(struct chunk *historyChunk);


