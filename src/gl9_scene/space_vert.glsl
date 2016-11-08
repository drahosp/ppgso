#version 150
// The inputs will be fed by the vertex buffer objects
in vec3 Position;
in vec2 TexCoord;

// This will be passed to the fragment shader
out vec2 FragTexCoord;

// Matrices as program attributes
uniform mat4 ModelMatrix;

void main() {
  // Copy the input to the fragment shader
  FragTexCoord = TexCoord;

  // Calculate the final position on screen
  gl_Position = ModelMatrix * vec4(Position, 1.0);
}
