#version 150
// Get vertex position from the vertex buffer
in vec2 pos;

uniform mat3 mat;

void main() {
  // Transform position using transformation matrix and input position
  vec3 new_pos = mat * vec3(pos,1);
  // The vertex shader needs to output a vec4 gl_Position so we fill in z and w to comply
  gl_Position = vec4(new_pos.xy, 0, 1.0);
}
