#include <glm/glm.hpp>

#include "camera.h"

using namespace std;
using namespace glm;
using namespace ppgso;

Camera::Camera(float fow, float ratio, float near, float far) {
  float fowInRad = (PI/180.0f) * fow;

  projectionMatrix = perspective(fowInRad, ratio, near, far);
}

void Camera::update() {
  viewMatrix = lookAt(position, position-back, up);
}
