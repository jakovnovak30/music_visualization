#include <assert.h>
#include <raylib.h>
#include <stdio.h>
#include <complex.h>
#include <dlfcn.h>

#include "common.h"
#include "process_audio.h"

__typeof__(audio_data) audio_data;
static void (*draw)(__typeof__(audio_data) *);

void reload_lib(void) {
  static void *handle = NULL;

  if(handle != NULL) dlclose(handle);

  handle = dlopen("bin/plug.so", RTLD_NOW);
  assert(handle != NULL);

  draw = (__typeof__(draw)) dlsym(handle, "draw");

  assert(draw != NULL);
  return;
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

  reload_lib();

  SetTargetFPS(FPS);
  while (!WindowShouldClose()) {
    PollInputEvents();
    UpdateMusicStream(music);

    if(IsKeyDown(KEY_R)) {
      reload_lib();
    }
    else if(IsKeyDown(KEY_Q)) {
      break;
    }

    if(draw != NULL) {
      for(int i=0;i < AUDIO_BUFFER_SIZE;++i) {
        audio_data.transformed[i] = audio_data.audio[i];
      }
      fft(audio_data.transformed, AUDIO_BUFFER_SIZE);

      draw(&audio_data);
    }
  }

  StopMusicStream(music);
  UnloadMusicStream(music);
  CloseWindow();
  return 0;
}
