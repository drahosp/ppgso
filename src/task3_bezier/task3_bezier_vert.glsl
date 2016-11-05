#version 150
// Get vertex position from the vertex buffer
in vec2 position;
//uniform vec3 color;
void main() {
  // The vertex shader needs to output a vec4 gl_Position so we fill in z and w to comply
  gl_Position = vec4(position, 0.0, 1.0);
}
