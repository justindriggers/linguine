#pragma once

#include "System.h"

#include <glm/vec2.hpp>

#include "renderer/Renderer.h"

namespace linguine {

class FriendlyAttackSystem : public System {
  public:
    FriendlyAttackSystem(EntityManager& entityManager,
                         Renderer& renderer)
        : System(entityManager), _renderer(renderer) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    Renderer& _renderer;

    void createProjectile(glm::vec2 location, glm::vec2 velocity, int32_t power);
};

}  // namespace linguine
