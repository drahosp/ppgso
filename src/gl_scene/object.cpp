#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "object.h"

Object::Object() {
  position = glm::vec3(0,0,0);
  scale = glm::vec3(1,1,1);
  rotation = glm::vec3(0,0,0);
  modelMatrix = glm::mat4(1.0f);
}

Object::~Object() {
}

void Object::GenerateModelMatrix() {
  modelMatrix =
          glm::translate(glm::mat4(1.0f), position)
          * glm::orientate4(rotation)
          * glm::scale(glm::mat4(1.0f), scale);
}

float Object::Rand(float min, float max) {
  return ((max - min) * ((float) rand() / (float) RAND_MAX)) + min;
}
