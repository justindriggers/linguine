#pragma once

#include "System.h"

#include <glm/vec3.hpp>

#include "renderer/Renderer.h"

namespace linguine {

class EnemySpawnSystem : public System {
  public:
    explicit EnemySpawnSystem(EntityManager& entityManager,
                              Renderer& renderer)
        : System(entityManager), _renderer(renderer) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    int _wave = 0;

    Renderer& _renderer;

    void createEnemy(glm::vec3 location);
};

}  // namespace linguine
