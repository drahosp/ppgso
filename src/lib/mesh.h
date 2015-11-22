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

class Mesh {
  GLuint vao;
  GLuint vbo, tbo, nbo;
  GLuint ibo;
  ShaderPtr program;
  TexturePtr texture;
  int mesh_indices_count;

  void initGeometry(const std::string &);
  void initTexture(const std::string &, unsigned int, unsigned int);

public:
  Mesh(ShaderPtr program, const std::string &obj);
  Mesh(ShaderPtr program, const std::string &obj, const TexturePtr texture);
  void Render();
};
typedef std::shared_ptr< Mesh > MeshPtr;

#endif // PPGSO_MESH_H
