#include <raylib.h>
#include <stdio.h>

#include "process_audio.h"
#include "common.h"

void draw(__typeof__(audio_data) *data) {
    static Shader shader = {0};
    static Texture texture = {0};
    if (shader.id == 0) {
      shader = LoadShader(NULL, "shaders/default.glsl");
      Image img = GenImageColor(WINDOW_WIDTH, WINDOW_HEIGHT, BLANK);
      texture = LoadTextureFromImage(img);
      UnloadImage(img);
    }

    if(data->index != AUDIO_BUFFER_SIZE-1) return;

    float max_amplitude = 0.0f;
    for(int i=0;i < AUDIO_BUFFER_SIZE;i++) {
      if(crealf(data->transformed[i]) > max_amplitude)
        max_amplitude = crealf(data->transformed[i]);
    }

    float transformed_data[AUDIO_BUFFER_SIZE / (2 * STEP)];
    for(int i=0;i < AUDIO_BUFFER_SIZE/(2*STEP);i++) {
      float acc = 0;

      for(int j=0;j < STEP;j++) {
        acc += data->transformed[i*STEP + j];
      }
      acc /= max_amplitude;
      transformed_data[i] = acc;

      #if 0
      int w = 2*STEP * WINDOW_WIDTH / data->index0
      DrawRectangle(w * i, WINDOW_HEIGHT/2 - acc*WINDOW_HEIGHT, w, acc * WINDOW_HEIGHT, RED);
      DrawRectangle(w * i, WINDOW_HEIGHT/2, w, acc * WINDOW_HEIGHT, BLUE);
      #endif
    }

    float time = GetTime();
    SetShaderValue(shader, GetShaderLocation(shader, "uTime"), &time, SHADER_UNIFORM_FLOAT);
    SetShaderValueV(shader, GetShaderLocation(shader, "frequencies"), transformed_data, SHADER_UNIFORM_FLOAT, AUDIO_BUFFER_SIZE / (2*STEP));

    ClearBackground(BLACK);
    BeginDrawing();
      BeginShaderMode(shader);
        DrawTexture(texture, 0, 0, WHITE);
      EndShaderMode();
    EndDrawing();
}
