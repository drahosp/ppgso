#pragma once
#include <memory>

#include <ppgso/ppgso.h>

#include "scene.h"
#include "object.h"

/*!
 * Simple asteroid object
 * This sphere object represents an instance of mesh geometry
 * It initializes and loads all resources only once
 * It will move down along the Y axis and self delete when reaching below -10
 */
class Asteroid final : public Object {
private:
  // Static resources (Shared between instances)
  static std::unique_ptr<ppgso::Mesh> mesh;
  static std::unique_ptr<ppgso::Shader> shader;
  static std::unique_ptr<ppgso::Texture> texture;

  // Age of the object in seconds
  float age{0.0f};

  // Speed and rotational momentum
  glm::vec3 speed;
  glm::vec3 rotMomentum;

  /*!
   * Split the asteroid into multiple pieces and spawn an explosion object.
   *
   * @param scene - Scene to place pieces and explosion into
   * @param explosionPosition - Initial position of the explosion
   * @param explosionScale - Scale of the explosion
   * @param pieces - Asteroid pieces to generate
   */
  void explode(Scene &scene, glm::vec3 explosionPosition, glm::vec3 explosionScale, int pieces);

public:
  /*!
   * Create new asteroid
   */
  Asteroid();

  /*!
   * Update asteroid
   * @param scene Scene to interact with
   * @param dt Time delta for animation purposes
   * @return
   */
  bool update(Scene &scene, float dt) override;

  /*!
   * Render asteroid
   * @param scene Scene to render in
   */
  void render(Scene &scene) override;

  /*!
   * Custom click event for asteroid
   */
  void onClick(Scene &scene) override;

private:
};

