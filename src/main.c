#include <assert.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>

#define WINDOW_TITLE "Music visualizer"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500
#define AUDIO_BUFFER_SIZE 4096
#define FPS 60
#define STEP 25

// fft implementation copied from rosseta code:
// https://rosettacode.org/wiki/Fast_Fourier_transform#C
typedef float complex cplx;
void _fft(cplx buf[], cplx out[], int n, int step) {
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

struct {
  float audio[AUDIO_BUFFER_SIZE];
  cplx transformed[AUDIO_BUFFER_SIZE];
  unsigned int index;
} audio_data;

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

int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "Wrong number of arguments!\n");
    return -1;
  }

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(AUDIO_BUFFER_SIZE);

  char *music_file = argv[1];
  printf("Playing file \"%s\" ...\n", music_file);

  Music music = LoadMusicStream(music_file);
  PlayMusicStream(music);
  AttachAudioStreamProcessor(music.stream, audio_callback);

  SetTargetFPS(FPS);
  while (!WindowShouldClose()) {
    PollInputEvents();
    UpdateMusicStream(music);
    ClearBackground(BLACK);

    if(audio_data.index != AUDIO_BUFFER_SIZE-1) continue;

    BeginDrawing();
    for(int i=0;i < AUDIO_BUFFER_SIZE;++i) {
      audio_data.transformed[i] = audio_data.audio[i];
    }
    fft(audio_data.transformed, AUDIO_BUFFER_SIZE);

    float max_amplitude = 0.0f;
    for(int i=0;i < AUDIO_BUFFER_SIZE;i++) {
      if(crealf(audio_data.transformed[i]) > max_amplitude)
        max_amplitude = crealf(audio_data.transformed[i]);
    }

    float acc = 0;
    for(int i=0;i < AUDIO_BUFFER_SIZE/2;i++) {
      acc += crealf(audio_data.transformed[i]);

      if(i % STEP == 0) {
        acc /= max_amplitude;
        int w = 2*STEP * WINDOW_WIDTH / audio_data.index;
        DrawRectangle(w * i/STEP, WINDOW_HEIGHT/2 - acc*WINDOW_HEIGHT, w, acc * WINDOW_HEIGHT, RED);
        DrawRectangle(w * i/STEP, WINDOW_HEIGHT/2, w, acc * WINDOW_HEIGHT, RED);
      }
    }
    EndDrawing();
  }

  StopMusicStream(music);
  UnloadMusicStream(music);
  CloseWindow();
  return 0;
}
