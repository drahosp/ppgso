#version 150
in vec2 Position;
in vec2 TexCoord;
out vec2 fragTexCoord;

void main() {
  fragTexCoord = vec2(TexCoord);  
  gl_Position = vec4(Position, 0.0, 1.0);
}
