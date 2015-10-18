#ifndef _OBJECT_3D_H_
#define _OBJECT_3D_H_

#include <iostream>
#include <vector>
#include <fstream>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tiny_obj_loader.h"

class Object3D {
  GLuint vao;
  GLuint vbo, tbo;
  GLuint ibo;
  GLuint program_id;
  GLuint texture_id;
  int mesh_indices_count;

  void initGeometry(const std::string &);
  void initTexture(const std::string &, unsigned int, unsigned int);
public:
  Object3D(GLuint, const std::string &, const std::string &, unsigned int, unsigned int);
  void render(glm::mat4);
};

#endif
