#include <raylib.h>
#include "process_audio.h"
#include "common.h"

void draw(__typeof__(audio_data) *data) {
    ClearBackground(BLACK);

    if(data->index != AUDIO_BUFFER_SIZE-1) return;

    BeginDrawing();

    float max_amplitude = 0.0f;
    for(int i=0;i < AUDIO_BUFFER_SIZE;i++) {
      if(crealf(data->transformed[i]) > max_amplitude)
        max_amplitude = crealf(data->transformed[i]);
    }

    float acc = 0;
    for(int i=0;i < AUDIO_BUFFER_SIZE/2;i++) {
      acc += crealf(data->transformed[i]);

      if(i % STEP == 0) {
        acc /= max_amplitude;
        int w = 2*STEP * WINDOW_WIDTH / data->index;
        DrawRectangle(w * i/STEP, WINDOW_HEIGHT/2 - acc*WINDOW_HEIGHT, w, acc * WINDOW_HEIGHT, RED);
        DrawRectangle(w * i/STEP, WINDOW_HEIGHT/2, w, acc * WINDOW_HEIGHT, BLUE);
      }
    }
    EndDrawing();
}
