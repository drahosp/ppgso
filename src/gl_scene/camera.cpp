#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

#define PI 3.14159265358979323846f

Camera::Camera(float fow, float ratio, float near, float far) {
  float fowInRad = (PI/180.0f) * fow;

  projectionMatrix = glm::perspective(fowInRad, ratio, near, far);

  up = glm::vec3(0,1,0);
  position = glm::vec3(0,0,0);
  back = glm::vec3(0,0,-1);
}

Camera::~Camera() {
}

void Camera::Update() {
  viewMatrix = glm::lookAt(position, position-back, up);
}
