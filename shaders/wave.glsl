#version 330
// Created by inigo quilez - iq/2013
// https://www.youtube.com/c/InigoQuilez
// https://iquilezles.org/
// link to original: https://www.shadertoy.com/view/Xds3Rr
// Adapted by Jakov Novak

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
uniform float wave[N];

void main()
{
  float w = 1.0f / N;
  float h = 1.0f / M;

  vec2 uv = fragTexCoord; // Tex coords
  vec2 uvReal = fragTexCoord * 2 - 1; // NDC

  int i = int(floor(uv.x / w));
  int j = int(floor(uvReal.y / h));
    
	float freq = frequencies[i];

	// convert frequency to colors
	vec3 col = vec3(freq, 4.0*freq*(1.0-freq), 1.0-freq) * freq;

    // add wave form on top	
	col += 1.0 -  smoothstep(0.0, 0.15, abs(wave[i] - uvReal.y));
	
	// output final color
	finalColor = vec4(col,1.0);
}
