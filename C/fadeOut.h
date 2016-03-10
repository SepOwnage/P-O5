
  struct fade_chunk{
      float fadeOutCoefficient;
      float currentFadeCoefficient;
  };

  void fadeOut(short buffer[BUFFERSIZE],struct fade_chunk * chunk);
