
/* represents buffer array elements: equivalent to BUFFERSIZE/2 temporal sample positions */
#define BUFFERSIZE 40

#define INPUTWAVFILE  "input.wav"
#define OUTPUTWAVFILE "output.wav"

#define VERBOSE

#define MAX_FILTERLENGTH_HALF 32
#define LENGTH_FILTER1_HALF 32
#define LENGTH_FILTER2_HALF 16
#define LENGTH_FILTER3_HALF 16
#define FILTER1_EVEN {-35, 74, -121, 188, -279, 397 ,-544, 726, -948, 1224,-1575, 2047,-2742, 3954,-6921,32767, 10057, -3848,2111, -1282, 798, -487, 279, -139, 47 , 10 ,-41, 52, -52, 46, -41, 32}
#define FILTER1_ODD {32,-41, 46,-52, 52,-41, 10, 47, -139, 279, -487,  798, -1282,  2111, -3848, 10057, 32767, -6921, 3954, -2742,  2047, -1575,  1224,  -948, 726, -544, 397, -279,188,-121, 74, -35}
#define FILTER2_EVEN {-186, 341,-709,1316, -2244,3752, -7020, 32767,  9099, -2830, 1082,-338,  21, 70,-52, 99}
#define FILTER2_ODD {99, -52, 70,21,-338,1082,-2830,9099, 32767, -7020,  3752, -2244,  1316,  -709,  341, -186} 
#define FILTER3_EVEN {-186, 341,-709,1316, -2244,3752, -7020, 32767,  9099, -2830, 1082,-338,  21, 70,-52, 99}
#define FILTER3_ODD {99, -52, 70,21,-338,1082,-2830,9099, 32767, -7020,  3752, -2244,  1316,  -709,  341, -186} 
#define MUS {.2, .2, .2, .2}
#define PHIS {.3, .3, .3, .3}
#define BUFFER_LENGTHS {5,5,5,5}

extern short filter1Even[LENGTH_FILTER1_HALF];
extern short filter1Odd[LENGTH_FILTER1_HALF];
extern short filter2Even[LENGTH_FILTER2_HALF];
extern short filter2Odd[LENGTH_FILTER2_HALF];
extern short filter3Even[LENGTH_FILTER2_HALF];
extern short filter3Odd[LENGTH_FILTER2_HALF];
