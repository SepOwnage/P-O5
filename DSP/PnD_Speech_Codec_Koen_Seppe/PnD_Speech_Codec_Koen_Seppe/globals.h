
typedef int size32_t;
typedef long size40_t;
typedef short size16_t;

/* represents buffer array elements: equivalent to BUFFERSIZE/2 temporal sample positions */
#define BUFFERSIZE 40
#define BUFFERSIZE_DIV4 10
#define BUFFERSIZE_DIV8 5
#define BUFFERSIZE_DIV2 20

#define INPUTWAVFILE  "f116.wav"
#define OUTPUTWAVFILE "output.wav"
#define COMPRESSEDFILE "compressed.dat"

//#define VERBOSE

#define MAX_FILTERLENGTH_HALF 32
#define LENGTH_FILTER1_HALF 32
#define LENGTH_FILTER2_HALF 16
#define FILTER1_EVEN {-35, 74, -121, 188, -279, 397, -544, 726, -948, 1224, -1575, 2047, -2742, 3954, -6921, 32767, 10057, -3848, 2111, -1282, 798, -487, 279, -139, 47, 10, -41, 52, -52, 46, -41, 32}
#define FILTER1_ODD {32, -41, 46, -52, 52, -41, 10, 47, -139, 279, -487, 798, -1282, 2111, -3848, 10057, 32767, -6921, 3954, -2742, 2047, -1575, 1224, -948, 726, -544, 397, -279, 188, -121, 74, -35}
#define FILTER2_EVEN {-186, 341, -709, 1316, -2244, 3752, -7020, 32767, 9099, -2830, 1082, -338, 21, 70, -52, 99}
#define FILTER2_ODD {99, -52, 70, 21, -338, 1082, -2830, 9099, 32767, -7020, 3752, -2244, 1316, -709, 341, -186} 
#define FILTER3_EVEN {1, -3, 10, -16, -118, 1185, -5465, 32767, 5706, -229, -134, 24, 0, 0, 0, 0}
#define FILTER3_ODD {0, 0, 0, 0, 24, -134, -229, 5706, 32767, -5465, 1185, -118, -16, 10, -3, 1} 

extern short filter1Even[LENGTH_FILTER1_HALF];
extern short filter1Odd[LENGTH_FILTER1_HALF];
extern short filter2Even[LENGTH_FILTER2_HALF];
extern short filter2Odd[LENGTH_FILTER2_HALF];
extern short filter3Even[LENGTH_FILTER2_HALF];
extern short filter3Odd[LENGTH_FILTER2_HALF];
