#ifndef PROCESS_AUDIO_H
#define PROCESS_AUDIO_H

#include "common.h"
#include <complex.h>
#include <math.h>

#define PI (atan2(1, 1)*4)

typedef float complex cplx;

void fft(cplx buf[], int n);
void audio_callback(void *buffer_data, unsigned int frames);

extern struct {
  float audio[AUDIO_BUFFER_SIZE];
  cplx transformed[AUDIO_BUFFER_SIZE];
  unsigned int index;
} audio_data;

#endif // PROCESS_AUDIO_H
