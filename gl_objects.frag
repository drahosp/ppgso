#version 150
// A texture is expected as program attribute
uniform sampler2D Texture;

// The vertex shader fill feed this input
in vec2 fragTexCoord;

// The final color
out vec4 FragmentColor;

void main() {
  // Lookup the color in Texture on coordinates given by fragTexCoord
  FragmentColor = texture(Texture, fragTexCoord);
}
