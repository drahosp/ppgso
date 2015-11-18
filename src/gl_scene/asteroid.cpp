#include "asteroid.h"

#include "object_frag.h"
#include "object_vert.h"

#define PI 3.14159265358979323846f

Asteroid::Asteroid() {
  scale *= Rand(1.0f, 3.0f);
  speed = glm::vec3(Rand(-2.0f, 2.0f), Rand(-5.0f, -10.0f), 0.0f);
  rotation = glm::vec3(Rand(-PI, PI), Rand(-PI, PI), Rand(-PI, PI));
  rotMomentum = glm::vec3(Rand(-PI, PI), Rand(-PI, PI), Rand(-PI, PI));

  // Initialize static resources if needed
  if (!shader) shader = ShaderPtr(new Shader{object_vert, object_frag});
  if (!texture) texture = TexturePtr(new Texture{"asteroid.rgb", 512, 512});
  if (!mesh) mesh = MeshPtr(new Mesh{shader, "asteroid.obj"});
}

Asteroid::~Asteroid() {
}

bool Asteroid::Update(Scene &scene, float dt){
  // Animate position according to time
  position += speed * dt;

  // Rotate the object
  rotation += rotMomentum * dt;

  // Delete if sphere is below y -10
  if (position.y<-10) return false;

  // Update matrix and time
  GenerateModelMatrix();
  return true;
}

void Asteroid::Render(Scene &scene) {
  shader->Use();

  // use camera
  shader->SetMatrix(scene.camera->projectionMatrix, "ProjectionMatrix");
  shader->SetMatrix(scene.camera->viewMatrix, "ViewMatrix");

  // render mesh
  shader->SetMatrix(modelMatrix, "ModelMatrix");
  shader->SetTexture(texture, "Texture");
  mesh->Render();
}

float Asteroid::Rand(float min, float max)  {
  return ((max-min)*((float)rand()/(float)RAND_MAX))+min;
}

// shared resources
MeshPtr Asteroid::mesh;
ShaderPtr Asteroid::shader;
TexturePtr Asteroid::texture;
