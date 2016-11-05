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
    Mesh(std::shared_ptr<Shader> &program, const std::string &obj);

    Mesh(Shader &program, const std::string &obj);

    void Render();
  };

}

#endif // PPGSO_MESH_H
