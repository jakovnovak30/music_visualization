#include <assert.h>
#include <endian.h>
#include <raylib.h>
#include <stdio.h>
#include <complex.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "process_audio.h"

__typeof__(audio_data) audio_data;
static void (*draw)(__typeof__(audio_data)*, const char*);

static void reload_lib(void) {
  static void *handle = NULL;

  if(handle != NULL) dlclose(handle);

  handle = dlopen("bin/plug.so", RTLD_NOW);
  assert(handle != NULL);

  draw = (__typeof__(draw)) dlsym(handle, "draw");

  assert(draw != NULL);
  return;
}

typedef struct {
  const char *shader_name;
  const char *music_path;
  // TODO: render method? terminal vs raylib?
} args_t;

#define ADD_ARG(_name, _alt_name, _code) \
  if(  !strcmp(argv[0], _name)       \
    || !strcmp(argv[0], _alt_name)) {\
    _code \
  }
#define HELP_MENU \
      "Usage: visualizer [OPTION] [VALUE]\n"\
      "Options:\n"\
      "--music or -m  : specify path to music file\n"\
      "--shader or -s : specify path to shader file (Optional)\n"\
      "--help or -h   : prints out this menu\n"\

static void shift_args(int *argc, char ***argv) {
  *argc = (*argc - 2);
  *argv = (*argv + 2);
}
static args_t parse_args(int argc, char **argv) {
  // the binary name is irrelevant
  argc--;argv++;

  args_t out;
  out.shader_name = "default";
  out.music_path = NULL;
  while(argc >= 1) {
    ADD_ARG("--help", "-h", {
      puts(HELP_MENU);
      exit(0);
    })

    ADD_ARG("--music", "-m", {
      out.music_path = argv[1];
    })
    ADD_ARG("--shader", "-s", {
;      out.shader_name = argv[1];
    })

    shift_args(&argc, &argv);
  }


  if(out.music_path == NULL) {
    puts(HELP_MENU);
    exit(1);
  }

  return out;
}

int main(int argc, char **argv) {
  args_t args = parse_args(argc, argv);

  /* init Raylib */
  char title[64];
  snprintf(title, 64, WINDOW_TITLE " - [%s]", args.music_path);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title);
  SetWindowState(FLAG_WINDOW_TOPMOST);
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(AUDIO_BUFFER_SIZE);
  SetTargetFPS(FPS);

  /* load music */
  Music music = LoadMusicStream(args.music_path);
  PlayMusicStream(music);
  AttachAudioStreamProcessor(music.stream, audio_callback);

  /* load draw function */
  reload_lib();

  /* main loop */
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

      draw(&audio_data, args.shader_name);
    }
  }

  /* cleanup stuff */
  StopMusicStream(music);
  UnloadMusicStream(music);
  CloseWindow();

  return 0;
}
