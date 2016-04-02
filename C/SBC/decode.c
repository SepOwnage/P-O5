#include "decode.h"


void copyToHistoryChunk(short *left_LL, short *left_LH, short *left_HL,
	short *right_LL, short *right_LH, short *right_HL,
	struct chunk *historyChunk) {
	int i;
	unsigned short position = historyChunk->position2;
	/*
	Puts seperate bands in appropiate bands in historyChunk, starting add historyChunk->position2
	and wrapping to the beginning of the arrays if the end is reached. historyChunk->position2 is
	NOT changed (<-> copyTolowerLayer).
	*/
	for (i = 0; i < BUFFERSIZE / 8; i++) {
		(historyChunk->leftLowEven)[position] = left_LL[i];
		(historyChunk->leftLowOdd)[position] = left_LH[i];
		(historyChunk->leftHighEven)[position] = left_HL[i];
		(historyChunk->leftHighOdd)[position] = 0;

		(historyChunk->rightLowEven)[position] = right_LL[i];
		(historyChunk->rightLowOdd)[position] = right_LH[i];
		(historyChunk->rightHighEven)[position] = right_HL[i];
		(historyChunk->rightHighOdd)[position] = 0;

		position = position % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF);
	}
}

void copyToUpperLayer(struct chunk *historyChunk) {
	/*
	Takes the samples from depth 2 and writes them to appropriate arrays of depth 1
	Using the FIFO structures to wrap around the endings as always. Both position1 and 
	position2 are left unchanged (<->copyToLowerLayer).
	*/
	int i;
	unsigned short position = historyChunk->position1;
	unsigned short positionPlus1 = (position + 1) % (BUFFERSIZE / 4 + LENGTH_FILTER1_HALF);
	for (i = 0; i < BUFFERSIZE / 8; i++) {   //   /8 ?/
		(historyChunk->leftEven)[position] = 
			(historyChunk->leftLowEven)[(historyChunk->position2 + i) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
		(historyChunk->leftEven)[positionPlus1] =
			(historyChunk->leftLowOdd)[(historyChunk->position2 + i) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
		
		(historyChunk->leftOdd)[position] =
			(historyChunk->leftHighEven)[(historyChunk->position2 + i) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
		(historyChunk->leftOdd)[positionPlus1] =
			(historyChunk->leftHighOdd)[(historyChunk->position2 + i) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];

		
		(historyChunk->rightEven)[position] =
			(historyChunk->rightLowEven)[(historyChunk->position2 + i) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
		(historyChunk->rightEven)[positionPlus1] =
			(historyChunk->rightLowOdd)[(historyChunk->position2 + i) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];

		(historyChunk->rightOdd)[position] =
			(historyChunk->rightHighEven)[(historyChunk->position2 + i) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];
		(historyChunk->rightOdd)[positionPlus1] =
			(historyChunk->rightHighOdd)[(historyChunk->position2 + i) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF)];


		position = (position + 2) % (BUFFERSIZE / 4 + LENGTH_FILTER1_HALF);
		positionPlus1 = (position + 1) % (BUFFERSIZE / 4 + LENGTH_FILTER1_HALF);
	}
}

void writeHistoryInBuffer(struct chunk *historyChunk, short *output) {
	int i;
	unsigned short position = historyChunk->position1;
	for (i = 0; i<BUFFERSIZE / 4; i++) {
		output[4 * i] = *((historyChunk->leftEven) + (historyChunk->position1));
		output[4 * i + 1] = *((historyChunk->rightEven) + (historyChunk->position1));
		output[4 * i + 2] = *((historyChunk->leftOdd) + (historyChunk->position1));
		output[4 * i + 3] = *((historyChunk->rightOdd) + (historyChunk->position1));
		position = (position + 1) % (BUFFERSIZE / 4 + LENGTH_FILTER1_HALF);
	}
}

void decode(short output[BUFFERSIZE],
		short *left_LL, short *left_LH, short *left_HL,
		short *right_LL, short *right_LH, short *right_HL,
		struct chunk *historyChunk) {

	//FOLLOWS ENCODE IN REVERSE ORDER
	
	//TODO: move outside of decode?  cleaner on amount of args
	copyToHistoryChunk(left_LL, left_LH, left_HL, right_LL, right_LH, right_HL, historyChunk);

	//TODO: filter 3??
	//TODO: skip fourth band? => faster but should be +- neglible compared to convolve
	combine(historyChunk->leftLowEven,
		historyChunk->leftLowOdd,
		historyChunk->leftLowEven,
		historyChunk->leftLowOdd,
		BUFFERSIZE / 8,
		historyChunk->position2,
		BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
		&(historyChunk->leftLowCombineTransfer));

	combine(historyChunk->leftHighEven,
		historyChunk->leftHighOdd,
		historyChunk->leftHighEven,
		historyChunk->leftHighOdd,
		BUFFERSIZE / 8,
		historyChunk->position2,
		BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
		&(historyChunk->leftHighCombineTransfer));

	combine(historyChunk->rightLowEven,
		historyChunk->rightLowOdd,
		historyChunk->rightLowEven,
		historyChunk->rightLowOdd,
		BUFFERSIZE / 8,
		historyChunk->position2,
		BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
		&(historyChunk->rightLowCombineTransfer));

	combine(historyChunk->rightLowEven,
		historyChunk->rightLowOdd,
		historyChunk->rightLowEven,
		historyChunk->rightLowOdd,
		BUFFERSIZE / 8,
		historyChunk->position2,
		BUFFERSIZE / 8 + LENGTH_FILTER2_HALF,
		&(historyChunk->rightHighCombineTransfer));

	//Before the convolve: move historyChunk->position2
	historyChunk->position2 = (historyChunk->position2 + BUFFERSIZE/8) % (BUFFERSIZE / 8 + LENGTH_FILTER2_HALF);

	convolve(historyChunk->leftLowEven, filter2Even,
		historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->leftLowEven, historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftLowOdd, filter2Odd,
		historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->leftLowOdd, historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftHighEven, filter2Even,
		historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->leftHighEven, historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, 16);
	convolve(historyChunk->leftHighOdd, filter2Odd,
		historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->leftHighOdd, historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, 16);

	convolve(historyChunk->rightLowEven, filter3Even,
		historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->rightLowEven, historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightLowOdd, filter3Odd,
		historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->rightLowOdd, historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightHighEven, filter3Even,
		historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->rightHighEven, historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, 16);
	convolve(historyChunk->rightHighOdd, filter3Odd,
		historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->rightHighOdd, historyChunk->position2, BUFFERSIZE / 8 + LENGTH_FILTER2_HALF, 16);
	
	copyToUpperLayer(historyChunk);

	combine(historyChunk->leftEven,
		historyChunk->leftOdd,
		historyChunk->leftEven,
		historyChunk->leftOdd,
		BUFFERSIZE / 4,
		historyChunk->position1,
		BUFFERSIZE / 4 + LENGTH_FILTER1_HALF,
		&(historyChunk->leftCombineTransfer));
	combine(historyChunk->rightEven,
		historyChunk->rightOdd,
		historyChunk->rightEven,
		historyChunk->rightOdd,
		BUFFERSIZE / 4,
		historyChunk->position1,
		BUFFERSIZE / 4 + LENGTH_FILTER1_HALF,
		&(historyChunk->rightCombineTransfer));

	(historyChunk->position1) = (historyChunk->position1 + BUFFERSIZE / 4) % (BUFFERSIZE / 4 + LENGTH_FILTER1_HALF);

	convolve(historyChunk->leftEven, filter1Even,
		historyChunk->position1, BUFFERSIZE / 4 + LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
		(historyChunk->leftEven), (historyChunk->position1), BUFFERSIZE / 4 + LENGTH_FILTER1_HALF, 17);
	convolve(historyChunk->leftOdd, filter1Odd,
		historyChunk->position1, BUFFERSIZE / 4 + LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
		(historyChunk->leftOdd), (historyChunk->position1), BUFFERSIZE / 4 + LENGTH_FILTER1_HALF, 17);
	convolve(historyChunk->rightEven, filter1Even,
		historyChunk->position1, BUFFERSIZE / 4 + LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
		(historyChunk->rightEven), (historyChunk->position1), BUFFERSIZE / 4 + LENGTH_FILTER1_HALF, 17);
	convolve(historyChunk->rightOdd, filter1Odd,
		historyChunk->position1, BUFFERSIZE / 4 + LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
		(historyChunk->rightOdd), (historyChunk->position1), BUFFERSIZE / 4 + LENGTH_FILTER1_HALF, 17);

	writeHistoryInBuffer(historyChunk, output);
}