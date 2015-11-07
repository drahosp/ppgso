#version 150
in vec3 VertexColor;

// This output is the final RGBA color of the fragment
out vec4 FragmentColor;

void main() {
  // Calculate the color based on the poisition of the fragment on screen
  // Note that the VertexColor is interpolated between the geometry vertices producing a smooth gradient transition
  FragmentColor = vec4(VertexColor, 1.0f);
}
