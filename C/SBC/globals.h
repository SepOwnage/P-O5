
/* represents buffer array elements: equivalent to BUFFERSIZE/2 temporal sample positions */
#define BUFFERSIZE 40

#define INPUTWAVFILE  "input.wav"
#define OUTPUTWAVFILE "output.wav"

#define VERBOSE

#define MAX_FILTERLENGTH_HALF 32
#define LENGTH_FILTER1_HALF 32
#define LENGTH_FILTER2_HALF 16
#define LENGTH_FILTER3_HALF 16
#define FILTER1_EVEN {-34, 58,-94,150, -230, 339, -479, 658, -881, 1163,-1525, 2011,-2723, 3953,-6937,32767, 9984,-3773,2034, -1206, 728, -426, 230, -104, 25, 19, -40,44, -39, 30,  -24, 27}
#define FILTER1_ODD {27, -24, 30,-39, 44, -40, 19, 25,-104, 230,-426, 728, -1206, 2034, -3773, 9984, 32767, -6937, 3953, -2723, 2011, -1525, 1163, -881, 658,-479, 339, -230, 150, -94, 58, -34}
#define FILTER2_EVEN {-130, 219, -532, 1112, -2061, 3636, -6996, 32767, 8874, -2603,  887, -207, -35, 65, -13, 60}
#define FILTER2_ODD {60, -13, 65, -35, -207, 887, -2603,  8874, 32767, -6996, 3636, -2061, 1112, -532, 219,-130} 
#define FILTER3_EVEN {-130, 219, -532, 1112, -2061, 3636, -6996, 32767, 8874, -2603,  887, -207, -35, 65, -13, 60}
#define FILTER3_ODD {60, -13, 65, -35, -207, 887, -2603,  8874, 32767, -6996, 3636, -2061, 1112, -532, 219,-130} 
#define MUS {.2, .2, .2, .2}
#define PHIS {.3, .3, .3, .3}
#define BUFFER_LENGTHS {5,5,5,5}

extern short filter1Even[LENGTH_FILTER1_HALF];
extern short filter1Odd[LENGTH_FILTER1_HALF];
extern short filter2Even[LENGTH_FILTER2_HALF];
extern short filter2Odd[LENGTH_FILTER2_HALF];
extern short filter3Even[LENGTH_FILTER2_HALF];
extern short filter3Odd[LENGTH_FILTER2_HALF];
