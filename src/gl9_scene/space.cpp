#include "space.h"
#include "scene.h"

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

Space::Space() {
  // Initialize static resources if needed
  if (!shader) shader = make_unique<Shader>(texture_vert_glsl, texture_frag_glsl);
  if (!texture) texture = make_unique<Texture>(image::loadBMP("stars.bmp"));
  if (!mesh) mesh = make_unique<Mesh>("quad.obj");
}

bool Space::update(Scene &scene, float dt) {
  // Offset for UV mapping, creates illusion of scrolling
  textureOffset.y -= dt/5;

  generateModelMatrix();
  return true;
}

void Space::render(Scene &scene) {
  // Disable writing to the depth buffer so we render a "background"
  glDepthMask(GL_FALSE);

  // NOTE: this object does not use camera, just renders the entire quad as is
  shader->use();

  // Pass UV mapping offset to the shader
  shader->setUniform("TextureOffset", textureOffset);

  // Render mesh, not using any projections, we just render in 2D
  shader->setUniform("ModelMatrix", modelMatrix);
  shader->setUniform("ViewMatrix", mat4{1.0f});
  shader->setUniform("ProjectionMatrix", mat4{1.0f});
  shader->setUniform("Texture", *texture);
  mesh->render();

  glDepthMask(GL_TRUE);
}

// shared resources
unique_ptr<Mesh> Space::mesh;
unique_ptr<Shader> Space::shader;
unique_ptr<Texture> Space::texture;
