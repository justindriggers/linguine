#pragma once

#include "entity/EntityManager.h"
#include "renderer/Renderer.h"

namespace linguine {

class ProjectileFactory {
  public:
    ProjectileFactory(EntityManager& entityManager, Renderer& renderer)
        : _entityManager(entityManager), _renderer(renderer) {}

    void create(glm::vec2 location, float speed, int32_t power);

  private:
    EntityManager& _entityManager;
    Renderer& _renderer;
};

}  // namespace linguine
