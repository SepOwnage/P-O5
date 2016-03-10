
#include <stdio.h>
#include "globals.h"
#include "fadeOut.h"

  void fadeOut(short buffer[BUFFERSIZE],struct fade_chunk * chunk)
  {

  int bufPos;
  float currentFadeCoefficient = chunk->currentFadeCoefficient;
  float fadeOutCoefficient = chunk->fadeOutCoefficient;

  for(bufPos=0;bufPos<BUFFERSIZE;bufPos+=2)
    {
    buffer[bufPos] = (int)(buffer[bufPos]*currentFadeCoefficient);
    buffer[bufPos+1] = (int)(buffer[bufPos+1]*currentFadeCoefficient);
    currentFadeCoefficient = currentFadeCoefficient*fadeOutCoefficient;
    }

  chunk->currentFadeCoefficient = currentFadeCoefficient;

  return;
  }
