
/* represents buffer array elements: equivalent to BUFFERSIZE/2 temporal sample positions */
#define BUFFERSIZE 40

#define INPUTWAVFILE  "input.wav"
#define OUTPUTWAVFILE "output.wav"

#define VERBOSE

#define MAX_FILTERLENGTH_HALF 32
#define LENGTH_FILTER1_HALF 32
#define LENGTH_FILTER2_HALF 16
#define LENGTH_FILTER3_HALF 16
#define FILTER1_EVEN {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32767}
#define FILTER1_ODD {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define FILTER2_EVEN {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32767}
#define FILTER2_ODD {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define FILTER3_EVEN {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32767}
#define FILTER3_ODD {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define MUS {.2, .2, .2, .2}
#define PHIS {.3, .3, .3, .3}
#define BUFFER_LENGTHS {5,5,5,5}

extern short filter1Even[LENGTH_FILTER1_HALF];
extern short filter1Odd[LENGTH_FILTER1_HALF];
extern short filter2Even[LENGTH_FILTER2_HALF];
extern short filter2Odd[LENGTH_FILTER2_HALF];
extern short filter3Even[LENGTH_FILTER2_HALF];
extern short filter3Odd[LENGTH_FILTER2_HALF];
