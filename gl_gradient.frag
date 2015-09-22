#version 150
// This output is the final RGBA color of the fragment
out vec4 FragmentColor;

void main() {
  // Calculate the color based on the poisition of the fragment on screen
  // Note that the gl_FragCoord vector has x and y with range <0, 512> as it is the screen width and height
  FragmentColor = vec4(gl_FragCoord.x/512.0f, gl_FragCoord.y/512.0f, 0.0f, 0.1f);
}
