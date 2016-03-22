#include "globals.h"

struct parameters{
	unsigned short *phi;
	unsigned short *mu;
	unsigned short *buffer_length;
}

struct start_values{
	unsigned short *initial_prediction;
	unsigned short *initial_stepsize;
}

void quantize(short samples[BUFFERSIZE],struct parameters *params, struct start_values *values){
	// Initialise

}

