#ifndef PPGSO_SHADER_H
#define PPGSO_SHADER_H

#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include "texture.h"

class Shader {
public:
  Shader(const std::string &vertex_shader_code, const std::string &fragment_shader_code);

  ~Shader();

  void Use();

  GLuint GetAttribLocation(const std::string &name);

  GLuint GetUniformLocation(const std::string &name);

  GLuint GetProgram();

  void SetFloat(float value, const std::string &name);
  void SetVector(glm::vec2 vector, const std::string &name);
  void SetVector(glm::vec3 vector, const std::string &name);
  void SetVector(glm::vec4 vector, const std::string &name);
  void SetTexture(const TexturePtr texture, const std::string &name);
  void SetMatrix(glm::mat4 matrix, const std::string &name);
  void SetMatrix(glm::mat3 matrix, const std::string &name);
private:
  GLuint program;
};
typedef std::shared_ptr< Shader > ShaderPtr;

#endif //  _PPGSO_SHADERPROGRAM_H
