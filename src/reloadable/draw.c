#include <raylib.h>
#include <stdio.h>

#include "process_audio.h"
#include "common.h"

void draw(__typeof__(audio_data) *data, const char *shader_name) {
    static Shader shader = {0};
    static Texture texture = {0};
    if (shader.id == 0) {
      char shader_path[64];
      snprintf(shader_path, 64, "shaders/%s.glsl", shader_name);
      shader = LoadShader(NULL, shader_path);
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
    float wave_data[AUDIO_BUFFER_SIZE / (2 * STEP)];
    for(int i=0;i < AUDIO_BUFFER_SIZE/(2*STEP);i++) {
      float acc_t = 0;
      float acc_w = 0;

      for(int j=0;j < STEP;j++) {
        acc_t += data->transformed[i*STEP + j];
        acc_w += data->audio[i*STEP + j];
      }
      transformed_data[i] = acc_t / max_amplitude;
      wave_data[i] = acc_w;
    }

    float time = GetTime();
    SetShaderValue(shader, GetShaderLocation(shader, "uTime"), &time, SHADER_UNIFORM_FLOAT);
    SetShaderValueV(shader, GetShaderLocation(shader, "frequencies"), transformed_data, SHADER_UNIFORM_FLOAT, AUDIO_BUFFER_SIZE / (2*STEP));
    SetShaderValueV(shader, GetShaderLocation(shader, "wave"), data->audio, SHADER_UNIFORM_FLOAT, AUDIO_BUFFER_SIZE / (2*STEP));

    ClearBackground(BLACK);
    BeginDrawing();
      BeginShaderMode(shader);
        DrawTexture(texture, 0, 0, WHITE);
      EndShaderMode();
    EndDrawing();
}
