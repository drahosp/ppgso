#version 150
// Get vertex position from the vertex buffer
in vec3 position;
in vec2 texCoord;

// Additional outputs
out vec2 fragTexCoord;

// Uniforms
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main() {
  // Just pass texture coordinates to fragment shader
  fragTexCoord = texCoord;

  // Perspective projection
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position,1);
}
