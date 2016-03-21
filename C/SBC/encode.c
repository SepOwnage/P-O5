#include "encode.h"
struct chunk{
//	unsigned short arraylength;
	signed short *left_odd;
	signed short *left_even;
	signed short *right_odd;
	signed short *right_even;
}

                                                short B[BUFFERSIZE];    
                  short BL[BUFFERSIZE/2];                                 short BR[BUFFERSIZE/2];
short BLE[BUFFERSIZE/4]; short BLO[BUFFERSIZE/4];          short BRE[BUFFERSIZE/4]; short BRO[BUFFERSIZE/4]; 
short CBLE[BUFFERSIZE/4*5]; short CBLO[BUFFERSIZE/4*5];    short CBRE[BUFFERSIZE/4*5]; short CBRO[BUFFERSIZE/4*5];
short CBLE_C[BUFFERSIZE/4*5]; short CBLO_C[BUFFERSIZE/4*5];short CBRE_C[BUFFERSIZE/4*5]; short CBRO_C[BUFFERSIZE/4*5]; //todo:  check with extra space usage from convolution
short L_L[BUFFERSIZE/4*5];  short L_H[BUFFERSIZE/4*5];     short R_L[BUFFERSIZE/4*5];  short R_H[BUFFERSIZE/4*5];

short L_L_E[BUFFERSIZE/4*5/2]; short L_L_O[BUFFERSIZE/4*5/2]; short R_L_E[BUFFERSIZE/4*5/2]; short R_L_O[BUFFERSIZE/4*5/2];
short L_H_E[BUFFERSIZE/4*5/2]; short L_H_O[BUFFERSIZE/4*5/2]; short R_H_E[BUFFERSIZE/4*5/2]; short R_H_O[BUFFERSIZE/4*5/2];

short L_L_E_C[BUFFERSIZE/4*5/2]; short L_L_O_C[BUFFERSIZE/4*5/2]; short R_L_E_C[BUFFERSIZE/4*5/2]; short R_L_O_C[BUFFERSIZE/4*5/2];
short L_H_E_C[BUFFERSIZE/4*5/2]; short L_H_O_C[BUFFERSIZE/4*5/2]; short R_H_E_C[BUFFERSIZE/4*5/2]; short R_H_O_C[BUFFERSIZE/4*5/2];

short L_LL[BUFFERSIZE/4*5/2]; short L_LH[BUFFERSIZE/4*5/2]; short R_LL[BUFFERSIZE/4*5/2]; short R_LH[BUFFERSIZE/4*5/2]; 
short L_HL[BUFFERSIZE/4*5/2]; short L_HH[BUFFERSIZE/4*5/2]; short R_HL[BUFFERSIZE/4*5/2]; short R_HH[BUFFERSIZE/4*5/2];

 
void convolve(short *in1, short *in2, short in1_l, short in2_l, short *out){
		

}

void encode(short buffer[BUFFERSIZE],struct chunk *input_chunk, struct chunk *output_chunk){
	
	B = buffer;
    splitLR(B, BL, BR);
	splitEO(BL, BLE, BLO);
	splitEO(BR, BRE, BRO);
	concat(BLE, CLE, CBLE);	
	concat(BLO, CLO, CBLO);
	concat(BRE, CRE, CBRE);	
	concat(BRO, CRO, CBRO);

	convolve(CBLE, filter1_even, CBLE_C);
	convolve(CBLO, filter1_odd, CBLO_C);
	convolve(CBRE, filter1_even, CBRE_C);
	convolve(CBRO, filter1_odd, CBRO_C);

	combine(CBLE_C, CBLO_C, L_L, L_H);
	combine(CBRE_C, CBRO_C, R_L, R_H);

	splitE0(L_L, L_L_E, L_L_O);
	splitE0(L_H, L_H_E, L_H_O);
	splitE0(R_L, R_L_E, R_L_O);
	splitE0(R_H, R_H_E, R_H_O);

	convolve(L_L_E, filter2_even, L_L_E_C);
	convolve(L_L_O, filter2_odd , L_L_O_C);
	convolve(L_H_E, filter2_even, L_H_E_C);
	convolve(L_H_O, filter2_odd , L_H_O_C);
	convolve(R_L_E, filter2_even, R_L_E_C);
	convolve(R_L_O, filter2_odd , R_L_O_C);
	convolve(R_H_E, filter2_even, R_H_E_C);
	convolve(R_H_O, filter2_odd , R_H_O_C);

	combine(L_L_E_C, L_L_O_C, L_LL, L_LH);
	combine(L_H_E_C, L_H_O_C, L_HL, L_HH);
	combine(R_L_E_C, R_L_O_C, R_LL, R_LH);
	combine(R_H_E_C, R_H_O_C, R_HL, R_HH);


//	separateInFourAndAppendInChunk(buffer,previous_chunk);
//	convolve(input_chunk->left_odd, filter, filterlength, output_chunk->left-odd);

}


