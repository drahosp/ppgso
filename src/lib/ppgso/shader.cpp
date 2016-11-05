#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "texture.h"
#include "shader.h"

using namespace std;
using namespace glm;
using namespace ppgso;

/*!
 * Compile and manage an GLSL program and its inputs.
 *
 * @param vertex_shader_code - String containing the source of the vertex shader.
 * @param fragment_shader_code - String containing the source of the fragment shader.
 */
Shader::Shader(const string &vertex_shader_code, const string &fragment_shader_code) {
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
    string vertex_shader_log((unsigned int) info_length, ' ');
    glGetShaderInfoLog(vertex_shader_id, info_length, nullptr,
                       &vertex_shader_log[0]);
    cout << "Error Compiling Vertex Shader ..." << endl;
    cout << vertex_shader_log << endl;
  }

  // Compile fragment shader
  auto fragment_shader_code_ptr = fragment_shader_code.c_str();
  glShaderSource(fragment_shader_id, 1, &fragment_shader_code_ptr, nullptr);
  glCompileShader(fragment_shader_id);

  // Check fragment shader log
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_length);
    string fragment_shader_log((unsigned long) info_length, ' ');
    glGetShaderInfoLog(fragment_shader_id, info_length, nullptr,
                       &fragment_shader_log[0]);
    cout << "Error Compiling Fragment Shader ..." << endl;
    cout << fragment_shader_log << endl;
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
    string program_log((unsigned long) info_length, ' ');
    glGetProgramInfoLog(program_id, info_length, nullptr, &program_log[0]);
    cout << "Error Linking Shader Program ..." << endl;
    cout << program_log << endl;
  }
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  program = program_id;
  Use();
}

Shader::~Shader() {
  glDeleteProgram( program );
}

/*!
 * Bind the shader program for use.
 */
void Shader::Use() {
  glUseProgram(program);
}

/*!
 * Get OpenGL attribute location for for the input specified by _name_
 *
 * @param name - Name of the shader program input variable.
 * @return - OpenGL attribute location number.
 */
GLuint Shader::GetAttribLocation(const string &name) {
  Use();
  return (GLuint) glGetAttribLocation(program, name.c_str());
}

/*!
 * Get OpenGL uniform location for for the input specified by _name_
 *
 * @param name - Name of the shader program input variable.
 * @return - OpenGL attribute location number.
 */
GLuint Shader::GetUniformLocation(const string &name) {
  Use();
  return (GLuint) glGetUniformLocation(program, name.c_str());
}

/*!
 * Set texture as an input for the shader program variable _name_
 *
 * @param texture - Texture to set input to.
 * @param name - Name of the shader program uniform input variable.
 */
void Shader::SetTexture(const shared_ptr<Texture> &texture, const string &name) {
  Use();
  auto texture_id = texture->GetTexture();
  auto uniform = GetUniformLocation(name.c_str());
  glUniform1i(uniform, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
}

/*!
 * Set texture as an input for the shader program variable _name_
 *
 * @param texture - Texture to set input to.
 * @param name - Name of the shader program uniform input variable.
 */
void Shader::SetTexture(Texture &texture, const string &name) {
  Use();
  auto texture_id = texture.GetTexture();
  auto uniform = GetUniformLocation(name.c_str());
  glUniform1i(uniform, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
}

/*!
 * Set matrix as an input for the shader program variable _name_
 *
 * @param matrix - Matrix to set input to.
 * @param name - Name of the shader program uniform input variable.
 */
void Shader::SetMatrix(mat4 matrix, const string &name) {
  Use();
  auto uniform = GetUniformLocation(name.c_str());
  glUniformMatrix4fv(uniform, 1, GL_FALSE, value_ptr(matrix));
}

/*!
 * Set matrix as an input for the shader program variable _name_
 *
 * @param matrix - Matrix to set input to.
 * @param name - Name of the shader program uniform input variable.
 */
void Shader::SetMatrix(mat3 matrix, const string &name) {
  Use();
  auto uniform = GetUniformLocation(name.c_str());
  glUniformMatrix3fv(uniform, 1, GL_FALSE, value_ptr(matrix));
}

/*!
 * Set a floating point value as an input for the shader program variable _name_
 *
 * @param value - Value to set input to.
 * @param name - Name of the shader program uniform input variable.
 */
void Shader::SetFloat(float value, const string &name) {
  Use();
  auto uniform = GetUniformLocation(name.c_str());
  glUniform1f(uniform, value);
}

/*!
 * Get OpenGL program identifier number.
 *
 * @return - OpenGL program identifer number.
 */
GLuint Shader::GetProgram() { return program; }


/*!
 * Set a vector as an input for the shader program variable _name_
 *
 * @param vector - Vector to set input to.
 * @param name - Name of the shader program uniform input variable.
 */
void Shader::SetVector(vec2 vector, const string &name) {
  Use();
  auto uniform = GetUniformLocation(name.c_str());
  glUniform2fv(uniform, 1, value_ptr(vector));
}

/*!
 * Set a vector as an input for the shader program variable _name_
 *
 * @param vector - Vector to set input to.
 * @param name - Name of the shader program uniform input variable.
 */
void Shader::SetVector(vec3 vector, const string &name) {
  Use();
  auto uniform = GetUniformLocation(name.c_str());
  glUniform3fv(uniform, 1, value_ptr(vector));
}

/*!
 * Set a vector as an input for the shader program variable _name_
 *
 * @param vector - Vector to set input to.
 * @param name - Name of the shader program uniform input variable.
 */
void Shader::SetVector(vec4 vector, const string &name) {
  Use();
  auto uniform = GetUniformLocation(name.c_str());
  glUniform4fv(uniform, 1, value_ptr(vector));
}
