#version 150
// This output is the final RGBA color of the fragment
in vec2 fragTexCoord;
uniform sampler2D tex;

out vec4 fragment;

void main() {
  fragment = texture(tex, fragTexCoord);
}
