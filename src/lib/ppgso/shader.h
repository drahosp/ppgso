#ifndef PPGSO_SHADER_H
#define PPGSO_SHADER_H

#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"

namespace ppgso {

  class Shader {
  public:

    /*!
     * Compile and manage an GLSL program and its inputs.
     *
     * @param vertex_shader_code - String containing the source of the vertex shader.
     * @param fragment_shader_code - String containing the source of the fragment shader.
     */
    Shader(const std::string &vertex_shader_code, const std::string &fragment_shader_code);

    ~Shader();

    /*!
     * Bind the shader program for use in OpenGL state.
     */
    void Use();

    /*!
     * Get OpenGL attribute location for for the input specified by "name"
     *
     * @param name - Name of the shader program input variable.
     * @return - OpenGL attribute location number.
     */
    GLuint GetAttribLocation(const std::string &name);

    /*!
     * Get OpenGL uniform location for for the input specified by "name"
     *
     * @param name - Name of the shader program input variable.
     * @return - OpenGL attribute location number.
     */
    GLuint GetUniformLocation(const std::string &name);

    /*!
     * Get OpenGL program identifier number.
     *
     * @return - OpenGL program identifer number.
     */
    GLuint GetProgram();

    /*!
     * Set a floating point value as an input for the shader program variable "name"
     *
     * @param value - Value to set input to.
     * @param name - Name of the shader program uniform input variable.
     */
    void SetFloat(float value, const std::string &name);

    /*!
     * Set a vector as an input for the shader program variable "name"
     *
     * @param vector - Vector to set input to.
     * @param name - Name of the shader program uniform input variable.
     */
    void SetVector(glm::vec2 vector, const std::string &name);

    /*!
     * Set a vector as an input for the shader program variable "name"
     *
     * @param vector - Vector to set input to.
     * @param name - Name of the shader program uniform input variable.
     */
    void SetVector(glm::vec3 vector, const std::string &name);

    /*!
     * Set a vector as an input for the shader program variable "name"
     *
     * @param vector - Vector to set input to.
     * @param name - Name of the shader program uniform input variable.
     */
    void SetVector(glm::vec4 vector, const std::string &name);

    /*!
     * Set texture as an input for the shader program variable "name"
     *
     * @param texture - Texture to set input to.task6_bezier_surface
     * @param name - Name of the shader program uniform input variable.
     */
    void SetTexture(const std::shared_ptr<Texture> &texture, const std::string &name);

    /*!
     * Set texture as an input for the shader program variable "name"
     *
     * @param texture - Texture to set input to.
     * @param name - Name of the shader program uniform input variable.
     */
    void SetTexture(Texture &texture, const std::string &name);

    /*!
     * Set matrix as an input for the shader program variable "name"
     *
     * @param matrix - Matrix to set input to.
     * @param name - Name of the shader program uniform input variable.
     */
    void SetMatrix(glm::mat4 matrix, const std::string &name);

    /*!
     * Set matrix as an input for the shader program variable "name"
     *
     * @param matrix - Matrix to set input to.
     * @param name - Name of the shader program uniform input variable.
     */
    void SetMatrix(glm::mat3 matrix, const std::string &name);

  private:
    GLuint program;
  };

}

#endif //  _PPGSO_SHADERPROGRAM_H
