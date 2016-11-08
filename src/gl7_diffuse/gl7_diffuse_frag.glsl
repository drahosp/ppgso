#version 150
// A texture is expected as program attribute
uniform sampler2D Texture;

// Direction of light
uniform vec3 LightDirection;

// The vertex shader fill feed this input
in vec2 FragTexCoord;

// Wordspace normal
in vec4 normal;

// The final color
out vec4 FragmentColor;

void main() {
  // Compute diffuse lighting
  float diffuse = max(dot(normal, vec4(normalize(LightDirection), 1.0f)), 0.0f);

  // Lookup the color in Texture on coordinates given by fragTexCoord and apply diffuse lighting
  FragmentColor = texture(Texture, FragTexCoord) * diffuse;
}
