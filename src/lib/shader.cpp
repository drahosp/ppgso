#include <iostream>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "texture.h"
#include "shader.h"

Shader::Shader(const std::string &vertex_shader_code, const std::string &fragment_shader_code) {
  // Create shaders
  auto vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  auto fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  auto result = GL_FALSE;
  auto info_length = 0;

  // Compile vertex shader
  auto vertex_shader_code_ptr = vertex_shader_code.c_str();
  glShaderSource(vertex_shader_id, 1, &vertex_shader_code_ptr, nullptr);
  glCompileShader(vertex_shader_id);

  // Check vertex shader log
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_length);
    std::string vertex_shader_log((unsigned int) info_length, ' ');
    glGetShaderInfoLog(vertex_shader_id, info_length, nullptr,
                       &vertex_shader_log[0]);
    std::cout << "Error Compiling Vertex Shader ..." << std::endl;
    std::cout << vertex_shader_log << std::endl;
  }

  // Compile fragment shader
  auto fragment_shader_code_ptr = fragment_shader_code.c_str();
  glShaderSource(fragment_shader_id, 1, &fragment_shader_code_ptr, nullptr);
  glCompileShader(fragment_shader_id);

  // Check fragment shader log
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_length);
    std::string fragment_shader_log((unsigned long) info_length, ' ');
    glGetShaderInfoLog(fragment_shader_id, info_length, nullptr,
                       &fragment_shader_log[0]);
    std::cout << "Error Compiling Fragment Shader ..." << std::endl;
    std::cout << fragment_shader_log << std::endl;
  }

  // Create and link the program
  auto program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glBindFragDataLocation(program_id, 0, "FragmentColor");
  glLinkProgram(program_id);

  // Check program log
  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  if (result == GL_FALSE) {
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_length);
    std::string program_log((unsigned long) info_length, ' ');
    glGetProgramInfoLog(program_id, info_length, nullptr, &program_log[0]);
    std::cout << "Error Linking Shader Program ..." << std::endl;
    std::cout << program_log << std::endl;
  }
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  program = program_id;
}

Shader::~Shader() {
  glDeleteProgram( program );
}

void Shader::Use() {
  glUseProgram(program);
}

GLuint Shader::GetAttribLocation(const std::string &name) {
  return glGetAttribLocation(program, name.c_str());
}

GLuint Shader::GetUniformLocation(const std::string &name) {
  return glGetUniformLocation(program, name.c_str());
}

void Shader::SetTexture(const TexturePtr texture, const std::string &name) {
  auto texture_id = texture->GetTexture();
  auto uniform = GetUniformLocation(name.c_str());
  glUniform1i(uniform, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Shader::SetMatrix(glm::mat4 matrix, const std::string &name) {
  auto uniform = GetUniformLocation(name.c_str());
  glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetMatrix(glm::mat3 matrix, const std::string &name) {
  auto uniform = GetUniformLocation(name.c_str());
  glUniformMatrix3fv(uniform, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetFloat(float value, const std::string &name) {
  auto uniform = GetUniformLocation(name.c_str());
  glUniform1f(uniform, value);
}

GLuint Shader::GetProgram() { return program; }

void Shader::SetVector(glm::vec2 vector, const std::string &name) {
  auto uniform = GetUniformLocation(name.c_str());
  glUniform2fv(uniform, 1, glm::value_ptr(vector));
}

void Shader::SetVector(glm::vec3 vector, const std::string &name) {
  auto uniform = GetUniformLocation(name.c_str());
  glUniform3fv(uniform, 1, glm::value_ptr(vector));
}

void Shader::SetVector(glm::vec4 vector, const std::string &name) {
  auto uniform = GetUniformLocation(name.c_str());
  glUniform4fv(uniform, 1, glm::value_ptr(vector));
}
