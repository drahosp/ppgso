#version 150
// A texture is expected as program attribute
uniform sampler2D Texture;

// The vertex shader fill feed this input
in vec2 FragTexCoord;

// Wordspace normal
in vec4 normal;

// The final color
out vec4 FragmentColor;

// Light direction in world coordinates
vec4 lightDirection = normalize(vec4(-0.5f,-0.5f,-0.5f,0.0f));

void main() {
  // Compute diffuse lighting
  float diffuse = max(dot(normal, lightDirection), 0.0f);

  // Lookup the color in Texture on coordinates given by fragTexCoord and apply diffuse lighting
  FragmentColor = texture(Texture, FragTexCoord) * diffuse ;
}
