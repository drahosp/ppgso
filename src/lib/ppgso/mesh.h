#ifndef PPGSO_MESH_H
#define PPGSO_MESH_H

#include <iostream>
#include <vector>
#include <fstream>
#include <memory>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"
#include "tiny_obj_loader.h"

namespace ppgso {

  class Mesh {
    GLuint vao;
    GLuint vbo, tbo, nbo;
    GLuint ibo;
    Shader &program;
    int mesh_indices_count;

    void initGeometry(const std::string &);

  public:

    /*!
     * Load 3D geometry from a na Wavefront .obj file.
     *
     * The shader program passed to the object will be bound to the geometry as follows:
     * vec3 Position - Vertex position
     * vec2 TexCoord - Texture coordinate
     * vec3 Normal - Normal vector
     *
     * @param program - Shader program to associate the loaded geometry with.
     * @param obj - File path to the obj file to load.
     */
    Mesh(std::shared_ptr<Shader> &program, const std::string &obj);

    /*!
     * Load 3D geometry from a na Wavefront .obj file.
     *
     * The shader program passed to the object will be bound to the geometry as follows:
     * vec3 Position - Vertex position
     * vec2 TexCoord - Texture coordinate
     * vec3 Normal - Normal vector
     *
     * @param program - Shader program to associate the loaded geometry with.
     * @param obj - File path to the obj file to load.
     */
    Mesh(Shader &program, const std::string &obj);

    /*!
     * Render the geometry associated with the mesh using glDrawElements.
     */
    void Render();
  };

}

#endif // PPGSO_MESH_H
