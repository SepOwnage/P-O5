#include "encode.h"

void decode(short output_buffer[BUFFERSIZE],
			short *left_LL, short *left_LH, short *left_HL, 
			short *right_LL, short *right_LH, short *right_HL,
			struct chunk *historyChunk);
void copyToHistoryChunk(short *left_LL, short *left_LH, short *left_HL,
	short *right_LL, short *right_LH, short *right_HL,
	struct chunk *historyChunk);
void writeHistoryInBuffer(struct chunk *historyChunk, short *output);
void copyToUpperLayer(struct chunk *historyChunk);