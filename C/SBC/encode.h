#include "globals.h"


struct encode_chunk{
	short prev_values[MAX_FILTERLENGTH];
}  

void encode(short buffer[BUFFERSIZE],struct encode_chunk * chunk);
