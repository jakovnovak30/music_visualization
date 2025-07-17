#include <raylib.h>
#include <stdio.h>

#include "process_audio.h"
#include "common.h"

void draw(__typeof__(audio_data) *data) {
    static Shader shader = {0};
    if (shader.id == 0) {
      shader = LoadShader("shaders/default.vert", "shaders/default.frag");
    }

    if(data->index != AUDIO_BUFFER_SIZE-1) return;

    float max_amplitude = 0.0f;
    for(int i=0;i < AUDIO_BUFFER_SIZE;i++) {
      if(crealf(data->transformed[i]) > max_amplitude)
        max_amplitude = crealf(data->transformed[i]);
    }

    ClearBackground(BLACK);
    BeginDrawing(); BeginShaderMode(shader);
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
    EndShaderMode(); EndDrawing();
}
