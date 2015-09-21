#version 150
uniform sampler2D Texture;
in vec2 fragTexCoord;
out vec4 FragmentColor;

void main() {
  FragmentColor = texture(Texture, fragTexCoord);
}
