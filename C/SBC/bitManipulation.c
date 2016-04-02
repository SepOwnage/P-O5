#include "globals.h"
#include "bitManipulation.h"

void parseSomeSamples(short *lowest, short *low, short *high){
    short short1; // LOWEST LOWEST  LOWEST  LOWEST  LOWEST  LOW     LOW     LOW     LOW     HIGH    HIGH    HIGH    LOWEST1  LOWEST1  LOWEST1
    short short2; // LOWEST1 LOWEST1  LOW1     LOW1     LOW1     LOW1     HIGH1    HIGH1    HIGH1    LOWEST2  LOWEST2  LOWEST2  LOWEST2 LOWEST2 LOW2
    short short3; // LOW2    LOW2     LOW2     HIGH2    HIGH2    HIGH2 etcetera

    short1 = (*lowest<<11) + ((*low&0b1111)<<7) + ((*high&0b111)<<3) + ((*(lowest+1)&0b11100)>>2);
    short2 = ((*(lowest+1)&0b11)<<14) + ((*(low+1)&0b1111)<<10) + ((*(high+1)&0b111)<<7) + ((*(lowest+2)&0b11111)<<1) + ((*(low+2)&0b1000)>>3);
}
