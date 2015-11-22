#ifndef PPGSO_OBJECT_H
#define PPGSO_OBJECT_H

#include <memory>
#include <list>
#include <map>

#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/detail/type_vec3.hpp>

#define PI 3.14159265358979323846f

// Forward declare a scene
class Scene;

// Abstract scene object interface
// All objects in the scene should be able to Update and Render
// Generally we also want to keep position, rotation and scale for each object to generate a modelMatrix
class Object {
public:
  Object();
  virtual ~Object();

  // Primary interface Update should update the objects modelMatrix and return true
  // If update returns false than the object will be removed from the scene
  virtual bool Update(Scene &scene, float dt) = 0;

  // Render needs to gernerate geometry using the modelMatrix and camera from scene
  virtual void Render(Scene &scene) = 0;

  // Object properties
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
  glm::mat4 modelMatrix;

protected:
  // Generate modelMatrix from properties
  void GenerateModelMatrix();
  // Random float generator
  float Rand(float min, float max);
};
typedef std::shared_ptr<Object> ObjectPtr;

#endif //PPGSO_OBJECT_H
