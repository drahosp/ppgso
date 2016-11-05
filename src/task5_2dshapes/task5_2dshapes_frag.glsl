#version 150
// This output is the final RGBA color of the fragment
out vec4 pixel;

uniform vec3 color;

void main() {
  pixel = vec4(color,1.0f);
}
