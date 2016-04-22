#include "synthesis.h"


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
	for (i = 0; i < BUFFERSIZE_DIV8; i++) {
		(historyChunk->leftLowEven)[position] = left_LL[i];
		(historyChunk->leftLowOdd)[position] = left_LH[i];
		(historyChunk->leftHighEven)[position] = left_HL[i];
		(historyChunk->leftHighOdd)[position] = 0;

		(historyChunk->rightLowEven)[position] = right_LL[i];
		(historyChunk->rightLowOdd)[position] = right_LH[i];
		(historyChunk->rightHighEven)[position] = right_HL[i];
		(historyChunk->rightHighOdd)[position] = 0;

		position = (position + 1) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF);
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
	unsigned short positionPlus1 = (position + 1) % (BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF);
	for (i = 0; i < BUFFERSIZE_DIV8; i++) {   //   /8 ?/
		(historyChunk->leftEven)[position] =
			(historyChunk->leftLowOdd)[(historyChunk->position2 + i) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF)];
		(historyChunk->leftEven)[positionPlus1] =
			(historyChunk->leftLowEven)[(historyChunk->position2 + i) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF)];
		
		(historyChunk->leftOdd)[position] =
			(historyChunk->leftHighOdd)[(historyChunk->position2 + i) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF)];
		(historyChunk->leftOdd)[positionPlus1] =
			(historyChunk->leftHighEven)[(historyChunk->position2 + i) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF)];
		
		
		(historyChunk->rightEven)[position] =
			(historyChunk->rightLowOdd)[(historyChunk->position2 + i) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF)];
		(historyChunk->rightEven)[positionPlus1] =
			(historyChunk->rightLowEven)[(historyChunk->position2 + i) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF)];
		
		(historyChunk->rightOdd)[position] =
			(historyChunk->rightHighOdd)[(historyChunk->position2 + i) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF)];
		(historyChunk->rightOdd)[positionPlus1] =
			(historyChunk->rightHighEven)[(historyChunk->position2 + i) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF)];
		

		position = (position + 2) % (BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF);
		positionPlus1 = (position + 1) % (BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF);
	}
}

void writeHistoryInBuffer(struct chunk *historyChunk, short *output) {
	int i;
	unsigned short position = historyChunk->position1;
	for (i = 0; i<BUFFERSIZE_DIV4; i++) {
		output[4 * i] = *((historyChunk->leftOdd) + position);
		output[4 * i + 1] = *((historyChunk->rightOdd) + position);
		output[4 * i + 2] = *((historyChunk->leftEven) + position);
		output[4 * i + 3] = *((historyChunk->rightEven) + position);
		position = (position + 1) % (BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF);
	}
}


void combineWithoutDelay(short *low_freq, short *high_freq, short *out_upper, short *out_lower,
	unsigned short length, unsigned short start, unsigned short arrayLength) {
	/*
	combines low and high frequency signals in QMF scheme to lower and higher signals (inputs to polyphase components of fitler).
	upper = low_frequency + high_frequency
	lower = low_frequency - high_frequency
	upper: start of array holding the upper signal
	lower: start of array holding the lower signal
	out_low: start of array in which low frequency band should be written
	out_high: start of array in which high frequency band should be written
	length: the amount of samples to process
	start: where in the arrays the current data starts and should be written
	arrayLength: the length of previous array. Writing or reading after this length
	corresponds to writing reading in the beginning of them (modulo on index, FIFO like)
	Can work in place
	*/
	int i;
	short position;
	int temp;
	for (i = 0; i<length; i++) {
		position = (start + i) % arrayLength;
		temp = low_freq[position];
		out_upper[position] = (short)((temp + high_freq[position]) / 2); 
		out_lower[position] = (short)((temp - high_freq[position]) / 2);
	}
}

void synthesis(short output[BUFFERSIZE],
		short *left_LL, short *left_LH, short *left_HL,
		short *right_LL, short *right_LH, short *right_HL,
		struct chunk *historyChunk) {

	//FOLLOWS ENCODE IN REVERSE ORDER
	
	//TODO: move outside of synthesis?  cleaner on amount of args
	copyToHistoryChunk(left_LL, left_LH, left_HL, right_LL, right_LH, right_HL, historyChunk);

	//TODO: filter 3??
	//TODO: skip fourth band? => faster but should be +- neglible compared to convolve
	combineWithoutDelay(historyChunk->leftLowEven,
		historyChunk->leftLowOdd,
		historyChunk->leftLowEven,
		historyChunk->leftLowOdd,
		BUFFERSIZE_DIV8,
		historyChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF);

	combineWithoutDelay(historyChunk->leftHighEven,
		historyChunk->leftHighOdd,
		historyChunk->leftHighEven,
		historyChunk->leftHighOdd,
		BUFFERSIZE_DIV8,
		historyChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF);

	combineWithoutDelay(historyChunk->rightLowEven,
		historyChunk->rightLowOdd,
		historyChunk->rightLowEven,
		historyChunk->rightLowOdd,
		BUFFERSIZE_DIV8,
		historyChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF);

	combineWithoutDelay(historyChunk->rightHighEven,
		historyChunk->rightHighOdd,
		historyChunk->rightHighEven,
		historyChunk->rightHighOdd,
		BUFFERSIZE_DIV8,
		historyChunk->position2,
		BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF);

	//Before the convolve: move historyChunk->position2
	historyChunk->position2 = (historyChunk->position2 + BUFFERSIZE_DIV8) % (BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF);

	convolve(historyChunk->leftLowEven, filter2Odd,
		historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->leftLowEven, historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftLowOdd, filter2Even,
		historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->leftLowOdd, historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftHighEven, filter3Odd,
		historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->leftHighEven, historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->leftHighOdd, filter3Even,
		historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->leftHighOdd, historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, 15);

	convolve(historyChunk->rightLowEven, filter3Odd,
		historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->rightLowEven, historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightLowOdd, filter3Even,
		historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->rightLowOdd, historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightHighEven, filter3Odd,
		historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->rightHighEven, historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, 15);
	convolve(historyChunk->rightHighOdd, filter3Even,
		historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, LENGTH_FILTER2_HALF,
		historyChunk->rightHighOdd, historyChunk->position2, BUFFERSIZE_DIV8 + LENGTH_FILTER2_HALF, 15);
	
	copyToUpperLayer(historyChunk);

	combineWithoutDelay(historyChunk->leftEven,
		historyChunk->leftOdd,
		historyChunk->leftEven,
		historyChunk->leftOdd,
		BUFFERSIZE_DIV4,
		historyChunk->position1,
		BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF);
	combineWithoutDelay(historyChunk->rightEven,
		historyChunk->rightOdd,
		historyChunk->rightEven,
		historyChunk->rightOdd,
		BUFFERSIZE_DIV4,
		historyChunk->position1,
		BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF);

	(historyChunk->position1) = (historyChunk->position1 + BUFFERSIZE_DIV4) % (BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF);

	convolve(historyChunk->leftEven, filter1Odd,
		historyChunk->position1, BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
		(historyChunk->leftEven), (historyChunk->position1), BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF, 14);
	convolve(historyChunk->leftOdd, filter1Even,
		historyChunk->position1, BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
		(historyChunk->leftOdd), (historyChunk->position1), BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF, 14);
	convolve(historyChunk->rightEven, filter1Odd,
		historyChunk->position1, BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
		(historyChunk->rightEven), (historyChunk->position1), BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF, 14);
	convolve(historyChunk->rightOdd, filter1Even,
		historyChunk->position1, BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF, LENGTH_FILTER1_HALF,
		(historyChunk->rightOdd), (historyChunk->position1), BUFFERSIZE_DIV4 + LENGTH_FILTER1_HALF, 14);

	writeHistoryInBuffer(historyChunk, output);
}
