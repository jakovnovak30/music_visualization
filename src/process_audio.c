#include "process_audio.h"

#include <string.h>
#include <assert.h>

// fft implementation copied from rosseta code:
// https://rosettacode.org/wiki/Fast_Fourier_transform#C
static void _fft(cplx buf[], cplx out[], int n, int step) {
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
void fft(cplx buf[], int n) {
	cplx out[n];
	for (int i = 0; i < n; i++) out[i] = buf[i];
 
	_fft(buf, out, n, 1);
}

void audio_callback(void *buffer_data, unsigned int frames) {
  float frame_data[frames];
  for(int i=0;i < frames*2;++i) {
    float l = ((float *) buffer_data)[i+0];
    float r = ((float *) buffer_data)[i+1];
    frame_data[i/2] = (l + r) / 2;
  }

  // | curr_data |    empty space     |
  //             | curr_frame |
  if(audio_data.index + frames < AUDIO_BUFFER_SIZE) {
    memcpy(audio_data.audio + audio_data.index, frame_data, sizeof(float) * frames);
    audio_data.index += frames;
  }
  // | curr_data | empty |
  //             |     new_data      |
  //             | new_1 |  new_2    |
  //
  // plan:
  // | curr_data | new_1 |
  // |old| new_1 | new_2 |
  // move new_1 to end, then shift whole thing to the left
  else if (audio_data.index < AUDIO_BUFFER_SIZE && frames < AUDIO_BUFFER_SIZE) {
    assert(frames < AUDIO_BUFFER_SIZE && "Should override whole buffer!");
    unsigned int len_1, len_2;
    len_1 = AUDIO_BUFFER_SIZE - audio_data.index - 1;
    len_2 = frames - len_1;

    memcpy(audio_data.audio + audio_data.index, frame_data, sizeof(float) * len_1);
    audio_data.index += len_1;
    assert(audio_data.index == AUDIO_BUFFER_SIZE-1);

    // shift everything by len_2 to the left, so we have len_2 space at the end
    memmove(audio_data.audio, audio_data.audio + len_2 - 1, sizeof(float) * (AUDIO_BUFFER_SIZE - len_2 - 1));
    memcpy(audio_data.audio + AUDIO_BUFFER_SIZE-len_2, frame_data, sizeof(float) * len_2);
  }
  // |       curr_data     |
  //                 | new |
  // |       old     | new |
  else if(audio_data.index == AUDIO_BUFFER_SIZE-1) {
    memmove(audio_data.audio, audio_data.audio + frames, sizeof(float) * (AUDIO_BUFFER_SIZE - frames));
    memcpy(audio_data.audio + AUDIO_BUFFER_SIZE - frames, frame_data, sizeof(float) * frames);
  }
  else {
    assert(false && "Edge case!");
  }
}
