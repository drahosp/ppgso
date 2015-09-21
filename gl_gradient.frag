#version 150
out vec4 FragmentColor;

void main() {
  FragmentColor = vec4(gl_FragCoord.x/512.0f, gl_FragCoord.y/512.0f, 0.0f, 1.0f);
}
