#include "globals.h"
#include "bitManipulation.h"

void parseSomeSamples(short *lowest, short *low, short *high){
    short short1; // LOWEST LOWEST  LOWEST  LOWEST  LOWEST  LOW     LOW     LOW     LOW     HIGH    HIGH    HIGH    LOWEST  LOWEST  LOWEST
    short short2; // LOWEST LOWEST  LOW     LOW     LOW     LOW     HIGH    HIGH    HIGH    LOWEST  LOWEST  LOWEST  LOWEST  LOWEST  LOW
    short short3; // LOW    LOW     LOW     HIGH    HIGH    HIGH etcetera...

    short1 = (*lowest<<11) + ((*low&0b1111)<<7) + ((*high&0b111)<<3) + ((*lowest[1]&0b11100)>>2);
}
