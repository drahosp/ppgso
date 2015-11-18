#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

Camera::Camera(float fow, float ratio, float near, float far) {
  projectionMatrix = glm::perspective(fow, ratio, near, far);

  up = glm::vec3(0,1,0);
  position = glm::vec3(0,0,0);
  back = glm::vec3(0,0,-1);
}

Camera::~Camera() {
}

void Camera::Update() {
  viewMatrix = glm::lookAt(position, position-back, up);
}
