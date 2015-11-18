#include "space.h"
#include "scene.h"

#include "space_vert.h"
#include "space_frag.h"

Space::Space() {
  offset = 0;
  // Z of 1 means back as there is no perspective projection applied during render
  position.z = 1;

  // Initialize static resources if needed
  if (!shader) shader = ShaderPtr(new Shader{space_vert, space_frag});
  if (!texture) texture = TexturePtr(new Texture{"stars.rgb", 512, 512});
  if (!mesh) mesh = MeshPtr(new Mesh{shader, "quad.obj"});
}

Space::~Space() {
}

bool Space::Update(Scene &scene, float dt) {
  offset -= dt/5;

  GenerateModelMatrix();
  return true;
}

void Space::Render(Scene &scene) {
  // NOTE: this objedt does not use camera
  shader->Use();

  // Animate texture offset
  shader->SetFloat(offset, "Offset");

  // use camera
  shader->SetMatrix(scene.camera->projectionMatrix, "ProjectionMatrix");
  shader->SetMatrix(scene.camera->viewMatrix, "ViewMatrix");

  // render mesh
  shader->SetMatrix(modelMatrix, "ModelMatrix");
  shader->SetTexture(texture, "Texture");
  mesh->Render();
}

// Static resources
MeshPtr Space::mesh;
ShaderPtr Space::shader;
TexturePtr Space::texture;
