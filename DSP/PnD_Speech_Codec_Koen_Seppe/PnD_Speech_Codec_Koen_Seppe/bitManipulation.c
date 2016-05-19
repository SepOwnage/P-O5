#include "globals.h"
#include "bitManipulation.h"

void compress30Samples(short *samples){
	/*
	expects samples to point to start of an array with (in this order):
	-5 samples of lowest frequency, left channel.  5 effective bits each
	-5 samples of lowest frequency, right channel.  5 effective bits each
	-5 samples of low frequency, left channel.  4 effective bits each
	-5 samples of low frequency, right channel.  4 effective bits each
	-5 samples of high frequency, left channel.  3 effective bits each
	-5 samples of high frequency, right channel.  3 effective bits each

	Compressed version is written in the first 15 bytes (not shorts!) in this same array;
	*/
    
	unsigned char i = 0;
	//for easier accessing of bytes (and not have to worry about big or little endian)
	unsigned char *output = (unsigned char *)samples;
	short lowest1, lowest2, low1, low2, high1, high2;

	unsigned char first, second, third;
	//L = lowest frequency bit, M = low frequency bit, H = high frequency bit
	// first  =  LLLLLMMM
	// second =  MHHHLLLL
	// third  =  LMMMMHHH

	for (; i < 5; i++) {
		lowest1 = samples[2 * i];
		lowest2 = samples[2 * i + 1];
		low1 = samples[10 + 2 * i];
		low2 = samples[10 + 2 * i + 1];
		high1 = samples[20 + 2 * i];
		high2 = samples[20 + 2 * i + 1];


		first = (lowest1 << 3 ) | ((low1 >> 1) & 7 );
		second = (low1 << 7) | ((high1 & 7) << 4) | ((lowest2 >> 1) & 15);
		third = (lowest2 << 7) | ((low2 & 15) << 3) | (high2 & 7);

		output[3 * i] = first;
		output[3 * i + 1] = second;
		output[3 * i + 2] = third;
	}

}

void decompress30samples(unsigned char *compressedSamples, short *output) {
	//compressedSamples: array of 15 bytes, holding 30 compressedSamples compressed with compress30Samples
	//output: array of 30 shorts where the output will be written.

	unsigned char i = 0;
	short lowest1, lowest2, low1, low2, high1, high2;

	unsigned char first, second, third;

	for (; i < 5; i++) {
		//L = lowest frequency bit, M = low frequency bit, H = high frequency bit
		// first  =  LLLLLMMM
		// second =  MHHHLLLL
		// third  =  LMMMMHHH

		first = compressedSamples[3 * i];
		second = compressedSamples[3 * i + 1];
		third = compressedSamples[3 * i + 2];

		lowest1 = ((short) ((first & 248)<<8))>>11;
		lowest2 = ((short)((((second & 15) << 1) | ((third & 128) >> 7)) << 11)) >> 11;
		low1 = ((short)((((first & 7) << 1) | ((second & 128) >> 7)) << 12)) >> 12;
		
		low2 = ((short)((third & 120) << 9)) >> 12;
		high1 = ((short)((second & 112) << 9)) >> 13;
		high2 = ((short)((third & 7) << 13)) >> 13;

		output[2 * i] = lowest1;
		output[2 * i + 1] = lowest2;
		output[10 + 2 * i] = low1;
		output[10 + 2 * i + 1] = low2;
		output[20 + 2 * i] = high1;
		output[20 + 2 * i + 1] = high2;
	}


}
