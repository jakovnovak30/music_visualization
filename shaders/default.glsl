#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

// Custom variables
#define PI 3.14159265358979323846
#define N (4096 / (8*2))
#define M 256

uniform float uTime = 0.0;
uniform float frequencies[N];

void main()
{
    float w = 1.0f / N;
    float h = 1.0f / M;

    vec2 uv = fragTexCoord; // Tex coords
    vec2 uvReal = fragTexCoord * 2 - 1; // NDC

    int i = int(floor(uv.x / w));
    int j = int(floor(uvReal.y / h));

    if(j < 0) {
      finalColor = vec4(1.f, 0.f, 0.f, 1.f);
    }
    else {
      finalColor = vec4(0.f, 1.f, 0.f, 1.f);
    }

    j = abs(j);
    if(j > abs(frequencies[i]) * 800)
      finalColor = vec4(0.f);
}
