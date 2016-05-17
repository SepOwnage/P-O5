#include "globals.h"
#include "quantizeStructs.h"

void quantize(short *quantized_differences, short *start_of_samples_array,
	unsigned char start_position_in_samples_array, unsigned char length_of_samples_array,
	unsigned char nb_samples_to_do, struct parameters *params, struct start_values *values);
