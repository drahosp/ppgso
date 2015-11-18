#version 150
// The inputs will be fed by the vertex buffer objects
in vec2 Position;
in vec2 TexCoord;

// This will be passed to the fragment shader
out vec2 FragTexCoord;

// Matrices as program attributes
uniform mat3 Transform;

void main() {
  // Copy the texture coordinates to the fragment shader
  FragTexCoord = TexCoord;

  // Apply transformation matrix using homogeneous coordinates
  vec3 transformedPosition = Transform * vec3(Position, 1.0);

  // Calculate the final position on screen, this needs to be a 3D homogeneous vector
  gl_Position = vec4(transformedPosition.x, transformedPosition.y, 0.0, 1.0);
}
