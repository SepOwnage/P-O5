#include "globals.h"
#include "quantizeTogether.h"


void quantizeMono(short * restrict quantized_differences_left, short * restrict start_of_samples_array_left,
	unsigned char start_position_in_samples_array_left, unsigned char length_of_samples_array_left,
	unsigned char nb_samples_to_do, struct parameters *params_left, struct start_values *values_left,
	short * restrict quantized_differences_right, short * restrict start_of_samples_array_right,
	unsigned char start_position_in_samples_array_right, unsigned char length_of_samples_array_right, struct start_values *values_right);
